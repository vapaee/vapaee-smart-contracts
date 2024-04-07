#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/base/modules/global.hpp>
#include <vapaee/token/modules/utils.hpp>
#include <vapaee/pool/utils.hpp>
#include <vapaee/pool/modules/handler.hpp>
#include <vapaee/pool/modules/liquidity.hpp>
#include <vapaee/pool/modules/swap.hpp>

namespace vapaee {

    CONTRACT telospooldex : public eosio::contract {

        private:
            #include <vapaee/pool/tables.all.hpp>
            #include <vapaee/base/tables.all.hpp>

        public:
            using contract::contract;

            string get_version() { return string("0.9.6"); } // telospooldex-v0.9.6

            telospooldex(name receiver, name code, datastream<const char*> ds) :
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver;  vapaee::current_version = get_version();  }

            // Global module
            ACTION init() {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::init() ------------------\n");
                vapaee::base::global::action_init();
            };

            ACTION cancelfund(name funder, uint64_t marketid) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::cancelfund() ------------------\n");
                vapaee::pool::liquidity::action_cancel_fund(funder, marketid);
            }

            ACTION takepart(name funder, uint64_t marketid, asset score) {
                MAINTENANCE();
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::takepart() ------------------\n");
                vapaee::pool::liquidity::action_withdraw_participation(funder, marketid, score);
            }

            ACTION selftransf(name from, name to, asset quantity, string memo) {
                MAINTENANCE();
                
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::selftransf() ------------------\n");
                require_auth(get_self());

                vapaee::pool::handler::handle_pool_transfer(
                    "dasntmatter"_n, to, quantity, memo, dex::utils::get_contract_for_token(quantity.symbol.code()));
            }

            [[eosio::on_notify("*::transfer")]]
            void htransfer(
                name from,
                name to,
                asset quantity,
                string memo
            ) {
                MAINTENANCE();
                PRINT("\nHANDLER telospooldex::htransfer() ------------------\n");

                PRINT(" vapaee::current_contract: ", vapaee::current_contract.to_string(), "\n");
                PRINT(" vapaee::pool::contract: ", vapaee::pool::contract.to_string(), "\n");

                check(vapaee::current_contract == vapaee::pool::contract, "vapaee::current_contract != vapaee::pool::contract");

                // skip handling transfers from this contract to outside
                if (from == vapaee::dex::contract)
                    return;
                
                vapaee::pool::handler::handle_pool_transfer(
                    from, to, quantity, memo, get_first_receiver());
            }

            // ACTION hotfix() {
            //     PRINT("\nACTION ",vapaee::current_contract.to_string(),"::hotfix() ------------------\n");
            //     require_auth(get_self());
            //     
            //     pools pool_table(vapaee::current_contract, vapaee::current_contract.value);
            //     for(auto it = pool_table.begin(); it != pool_table.end(); it = pool_table.begin()) {
            //         pool_table.erase(it);
            //     }
            // }

            //ACTION hotfix() {
            //    require_auth(get_self());
            //    PRINT("\nACTION ",vapaee::current_contract.to_string(),"::hotfix2() ------------------\n");
            //    pools pool_markets(get_self(), get_self().value);

                // for(auto it = pool_markets.begin(); it != pool_markets.end(); it = pool_markets.begin()) {
                //     pool_markets.erase(it);
                // }

                //pool_markets.emplace(get_self(), [&](auto & row) {
                //    row.market_id = 0;
                //    row.commodity_reserve = asset(99989056204, symbol("CNT", 4));
                //    row.currency_reserve = asset(1000110000, symbol("EUROT", 4));
                //    row.total_participation = asset(100000000, symbol("PART", 8));
                //    row.buyfee = asset(500000, symbol("FEE", 8));
                //    row.sellfee = asset(500000, symbol("FEE", 8));
                //});
                //pool_markets.emplace(get_self(), [&](auto & row) {
                //    row.market_id = 4;
                //    row.commodity_reserve = asset(59960037, symbol("MULITA", 2));
                //    row.currency_reserve = asset(14774090, symbol("EUROT", 4));
                //    row.total_participation = asset(100000000, symbol("PART", 8));
                //    row.buyfee = asset(500000, symbol("FEE", 8));
                //    row.sellfee = asset(500000, symbol("FEE", 8));
                //});
                //pool_markets.emplace(get_self(), [&](auto & row) {
                //    row.market_id = 8;
                //    row.commodity_reserve = asset(99910558134084, symbol("TIPS", 6));
                //    row.currency_reserve = asset(100090023, symbol("EUROT", 4));
                //    row.total_participation = asset(100000000, symbol("PART", 8));
                //    row.buyfee = asset(500000, symbol("FEE", 8));
                //    row.sellfee = asset(500000, symbol("FEE", 8));
                //});
                //pool_markets.emplace(get_self(), [&](auto & row) {
                //    row.market_id = 10;
                //    row.commodity_reserve = asset(99189968615270, symbol("DIVERSE", 6));
                //    row.currency_reserve = asset(100820833, symbol("EUROT", 4));
                //    row.total_participation = asset(100000000, symbol("PART", 8));
                //    row.buyfee = asset(500000, symbol("FEE", 8));
                //    row.sellfee = asset(500000, symbol("FEE", 8));
                //});
                //pool_markets.emplace(get_self(), [&](auto & row) {
                //    row.market_id = 12;
                //    row.commodity_reserve = asset(25605540, symbol("TLOS", 4));
                //    row.currency_reserve = asset(3914177, symbol("EUROT", 4));
                //    row.total_participation = asset(100000000, symbol("PART", 8));
                //    row.buyfee = asset(500000, symbol("FEE", 8));
                //    row.sellfee = asset(500000, symbol("FEE", 8));
                //});
                //pool_markets.emplace(get_self(), [&](auto & row) {
                //    row.market_id = 14;
                //    row.commodity_reserve = asset(1228562610122, symbol("ACORN", 4));
                //    row.currency_reserve = asset(100491405, symbol("EUROT", 4));
                //    row.total_participation = asset(100000000, symbol("PART", 8));
                //    row.buyfee = asset(500000, symbol("FEE", 8));
                //    row.sellfee = asset(500000, symbol("FEE", 8));
                //});
                //pool_markets.emplace(get_self(), [&](auto & row) {
                //    row.market_id = 16;
                //    row.commodity_reserve = asset(999041364, symbol("VPE", 6));
                //    row.currency_reserve = asset(100096449, symbol("EUROT", 4));
                //    row.total_participation = asset(100000000, symbol("PART", 8));
                //    row.buyfee = asset(500000, symbol("FEE", 8));
                //    row.sellfee = asset(500000, symbol("FEE", 8));
                //});
                //pool_markets.emplace(get_self(), [&](auto & row) {
                //    row.market_id = 18;
                //    row.commodity_reserve = asset(1428503237299, symbol("BTTM", 4));
                //    row.currency_reserve = asset(10000479839, symbol("EUROT", 4));
                //    row.total_participation = asset(100000000, symbol("PART", 8));
                //    row.buyfee = asset(500000, symbol("FEE", 8));
                //    row.sellfee = asset(500000, symbol("FEE", 8));
                //});


            //}

    };  // contract class

};  // namespace vapaee
