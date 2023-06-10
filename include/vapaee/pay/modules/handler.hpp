#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pay/errors.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <vapaee/pay/modules/hub.hpp>
#include <vapaee/pay/modules/liquid.hpp>
#include <vapaee/pay/modules/billing.hpp>

namespace vapaee {
    namespace pay {
        namespace handler {

            inline name get_self() {
                return vapaee::pay::contract;
            }

            name check_deposit_memo(name from, const vector<string>& memo_tokens, const string& memo) {
                // parse the second part of the memo to extract the loan id to be cancelled
                name receiver = from;
                if (memo_tokens.size() == 2) {
                    receiver = vapaee::utils::check_name_from_string(memo_tokens[1]);
                }

                return receiver;
            }

            void withdraw_foreign_asset(const asset& quantity, const string& skip_memo) {
                action(
                    permission_level{contract,name("active")},
                    vapaee::token::contract,
                    name("withdraw"),
                    std::make_tuple(get_self(), quantity, skip_memo)
                ).send();
            }

            void handle_pay_transfer(
                const name& from,
                const name& to,
                const asset& quantity,
                const string& memo,
                const name& token_contract
            ) {
                PRINT("vapaee::pay::handler::handle_pay_transfer()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");
                PRINT(" token_contract: ", token_contract.to_string(), "\n");

                // skip handling some cases
                if (vapaee::base::global::
                    handler_should_ignore_transfer(from, to, quantity, memo, token_contract)
                ) {
                    PRINT("vapaee::pay::handler::handle_pay_transfer()... skipping\n");
                    return;
                }

                // get the real token contract
                name original_token_contract = token_contract;
                symbol_code token = quantity.symbol.code();
                bool is_foreign = false;
                if (
                    original_token_contract == vapaee::token::contract &&           // comes from vapaeetokens
                    vapaee::token::wrapper::is_token_registered_as_foreign(token)   // is foreign
                ) {
                    is_foreign = true;
                    original_token_contract = vapaee::token::wrapper::get_token_foreign_contract(token);
                }

                string skip_memo = string("skip ") +
                    "transfer(" +
                        from.to_string() + ", " +
                        to.to_string() + ", " +
                        quantity.to_string() + ", " +
                        memo + ")";

                // skip if it is a debit from vapaeetokens.
                if (memo.find(string("vapaeetokens debit service:")) != std::string::npos) {
                    if (is_foreign) {
                        // it is a debit so it was already handled. But it is foreign
                        // so, me must withdraw it to have liquidity for next transfer (on the foreign contract)
                        withdraw_foreign_asset(quantity, skip_memo + " reason: debit already handled");
                    }
                    return;
                }

                // check if pay is valid (pay is registered, tradeable, genuine and not blacklisted)
                vapaee::dex::security::aux_check_token_ok(quantity.symbol, original_token_contract, "ERR-HPAYT-01");

                // parsing the memo and cheking integrity
                vector<string> memo_parts = split(memo, " ");
                check(memo_parts.size() > 0, create_error_string1(ERROR_HPT_2, memo).c_str());

                // safety check if first part of memo is valid
                name header = vapaee::utils::check_name_from_string(memo_parts[0]);

                PRINT(" > header: ", header.to_string(), "\n");
                switch(header.value) {

                    // Do not proccess the transfer
                    case name("skip").value: {
                        break;
                    }

                    case name("droponpool").value: {
                        name poollabel = vapaee::utils::check_name_from_string(memo_parts[1]);

                        action(
                            permission_level{get_self(), "active"_n},
                            get_self(),
                            "droponpool"_n,
                            make_tuple(
                                quantity,
                                poollabel
                            )
                        ).send();  

                        break;
                    }

                    // Perform payment
                    case name("pay").value: {

                        string payhub_id = memo.substr(4);                        
                        vapaee::pay::hub::handle_payhub_payment(quantity, payhub_id, memo);

                        break;
                    }

                    // Perform payment with invoice
                    case name("invoice").value: {

                        string payhub_id = memo_parts[1];
                        string fiat_str = memo_parts[2] + " " + memo_parts[3];
                        asset fiat = vapaee::utils::check_asset_from_string(fiat_str);

                        int len = memo_parts[0].size() + memo_parts[1].size() + memo_parts[2].size() + memo_parts[3].size() + 4;
                        string invoice_memo = memo.substr(len);
                        vapaee::pay::billing::handle_invoice(from, quantity, fiat, payhub_id, invoice_memo);

                        break;
                    }

                    // Add liquidity in a leakpool
                    case name("liquidity").value: {

                        string leakpool_id = memo_parts[1];
                        vapaee::pay::liquid::handle_adding_liquidity(quantity, leakpool_id);

                        break;
                    }

                    default: {
                        check(false, create_error_string1("ERR-HPAYT-02", memo).c_str());
                    }


                }
                
                if (is_foreign && name("skip").value != header.value) {
                    // asset comes from vapaeetokens but is not native
                    // so we must withdraw it and skip the handler
                    // Then we treat this income as it were coming from the original foreign contract
                    withdraw_foreign_asset(quantity, skip_memo + " reason: transfer already handled");
                }
                
                PRINT("vapaee::pay::handler::handle_pay_transfer()...\n");

            }


        };     
    };
};
