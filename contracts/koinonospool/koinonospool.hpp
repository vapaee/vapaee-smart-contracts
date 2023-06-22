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

            string get_version() { return string("0.1.0"); } // koinonospool-0.1.0 - first version

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

            asset extract_currenty_from_memo(string memo) {
                vector<string> memo_tokens = split(memo, ",");
                check(memo_tokens.size() > 0, create_error_string1(ERROR_HPT_2, memo).c_str());
                return vapaee::utils::check_asset_from_string(memo_tokens[0]);
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
                if (from == vapaee::dex::contract)
                    return;

// vapaee::pool::swap::self = vapaee::current_contract;
// vapaee::pool::utils::self = vapaee::current_contract;
// vapaee::pool::liquidity::self = vapaee::current_contract;

                // we se the aproapiated fee for this swap
// if (quantity.symbol.code() == eosio::symbol_code("KOINE")) {
//     // if user is selling KOINE, we charge 0.1% of KOINE
//     vapaee::pool::utils::swap_fee = asset(100000, fee_symbol); // 0.1%
// } else {
//     // if user is buying KOINE, we don't charge anything
//     vapaee::pool::utils::swap_fee = asset(0, fee_symbol); // 0.0%
// }

                // perform the swap
                name command = vapaee::pool::handler::handle_pool_transfer(
                    from, to, quantity, memo, get_first_receiver());

                // Now after performing the swap, we burn the KOINE fee for ever
                if (command == name("openpool.v1")) {
                    // if (vapaee::pool::utils::swap_fee.amount > 0) {
                        string burn_memo = string("Burning %0.1 of ") + quantity.to_string() + " tokens for ever ";
                        vapaee::token::utils::send_burn_tokens(quantity, burn_memo, vapaee::token::contract);
check(false, burn_memo.c_str());
                    // }
                }

            }

    };  // contract class

};  // namespace vapaee
