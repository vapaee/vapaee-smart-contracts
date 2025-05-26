#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/base/modules/global.hpp>
#include <vapaee/token/modules/utils.hpp>
#include <vapaee/pool/utils.hpp>
#include <vapaee/pool/modules/handler.hpp>
#include <vapaee/pool/modules/liquidity.hpp>
#include <vapaee/pool/modules/swap.hpp>

namespace vapaee {

    CONTRACT koinonospool : public eosio::contract {

        private:
            #include <vapaee/pool/tables.all.hpp>
            #include <vapaee/base/tables.all.hpp>

        public:
            using contract::contract;

            string get_version() { return string("0.1.2"); }

            koinonospool(name receiver, name code, datastream<const char*> ds) :
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

            asset extract_currency_from_memo(string memo) {
                vector<string> memo_tokens = split(memo, ",");
                check(memo_tokens.size() > 0, create_error_string1(ERROR_HPT_2, memo).c_str());
                return vapaee::utils::check_asset_from_string(memo_tokens[0]);
            }

            ACTION clear() {
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

            [[eosio::on_notify("*::transfer")]]
            void htransfer(
                name from,
                name to,
                asset quantity,
                string memo
            ) {
                MAINTENANCE();
                PRINT("\nHANDLER",vapaee::current_contract.to_string(),"::htransfer() ------------------\n");

                PRINT(" vapaee::current_contract: ", vapaee::current_contract.to_string(), "\n");
                PRINT(" vapaee::kpool::contract: ", vapaee::kpool::contract.to_string(), "\n");

                check(vapaee::current_contract == vapaee::kpool::contract, "vapaee::current_contract != vapaee::kpool::contract");

                // skip handling transfers from this contract to outside
                if (to != vapaee::current_contract)
                    return;

                vapaee::pool::utils::swap_sellfee = asset(100000, fee_symbol); // 0.1%
                vapaee::pool::utils::swap_buyfee = asset(0, fee_symbol); // 0.0%

                // perform the swap
                string result = vapaee::pool::handler::handle_pool_transfer(
                    from, to, quantity, memo, get_first_receiver());

                vector<string> parts = split(result, ",");
                name command = vapaee::utils::check_name_from_string(parts[0]);

                // Now after performing the swap, we burn the KOINE fee for ever
                if (command == name("openpool.v1")) {
                    asset fees = vapaee::utils::check_asset_from_string(parts[1]);
                    if (fees.amount > 0) {
                        string burn_memo = string("Burning ") + fees.to_string() + " tokens (%0.1 of " + quantity.to_string() + ") for ever ";
                        vapaee::token::utils::send_burn_tokens(fees, burn_memo, vapaee::token::contract);
                    }
                }

            }

    };  // contract class

};  // namespace vapaee
