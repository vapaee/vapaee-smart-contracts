#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/base/modules/global.hpp>
#include <vapaee/dex/dispatcher.spp>
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

            string get_version() { return string("0.9.3"); } // telospooldex

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

            HANDLER htransfer(
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

    };  // contract class

};  // namespace vapaee
