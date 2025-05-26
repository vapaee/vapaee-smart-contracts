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

            ACTION clear()  {
                PRINT("\nACTION ", vapaee::current_contract.to_string(), "::clear() ------------------\n");
                require_auth(get_self()); // Only the contract itself can call this

                // Iterate over all pools in scope = contract
                pools pools(get_self(), get_self().value);
                for (auto pit = pools.begin(); pit != pools.end(); ) {
                    uint64_t market = pit->market_id;

                    // Clear partscore (scope = market)
                    partscore parts(get_self(), market);
                    for (auto itp = parts.begin(); itp != parts.end(); itp = parts.erase(itp));

                    // Clear fundhistory (scope = market)
                    fundhistory fhs(get_self(), market);
                    for (auto itf = fhs.begin(); itf != fhs.end(); itf = fhs.erase(itf));

                    // Clear fundattempts (scope = market)
                    fundattempts fas(get_self(), market);
                    for (auto itfa = fas.begin(); itfa != fas.end(); itfa = fas.erase(itfa));

                    // Erase the pool itself
                    pit = pools.erase(pit);
                }
            }


    };  // contract class

};  // namespace vapaee
