#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/token/errors.hpp>
#include <vapaee/dex/modules/security.hpp>

namespace vapaee {
    namespace token {
        namespace handler {

            inline name get_self() {
                return vapaee::token::contract;
            }

            void handle_token_transfer(name from, name to, asset quantity, string memo, name tokencontract) {
                PRINT("vapaee::token::handler::handle_token_transfer()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");
                PRINT(" tokencontract: ", tokencontract.to_string(), "\n");

                if (vapaee::base::global::
                    handler_should_ignore_transfer(from, to, quantity, memo, tokencontract)
                ) return;

                if (vapaee::current_contract != vapaee::token::contract) return;


                // Ahora queremos permitir no solo que el usairo pueda depositar tokens con el memo "deposit"
                // si no que también pueda depositar tokens y al mismo tiempo crear un allowance del mismo monto para un tercero con el memo "allowance accountname"

                // parsing the memo and cheking integrity
                vector<string> memo_parts = split(memo, " ");
                check(memo_parts.size() > 0, create_error_string1(ERROR_HTT_1, memo).c_str());

                // safety check if first part of memo is valid
                name header = vapaee::utils::check_name_from_string(memo_parts[0]);

                PRINT(" > header: ", header.to_string(), "\n");
                switch(header.value) {

                    // Do not proccess the transfer
                    case name("skip").value: {
                        break;
                    }

                    // Perform a deposit for the user
                    case name("deposit").value: {

                        string memo = string("You deposited ") + quantity.to_string() + " tokens";
                        // if memo_parts.size() > 1 then we have a memo for the deposit
                        if (memo_parts.size() > 1) {
                            memo = memo_parts[1];
                            for (int i=2; i<memo_parts.size(); i++) {
                                memo += " " + memo_parts[i];
                            }
                        }

                        action(
                            permission_level{get_self(), "active"_n},
                            get_self(),
                            "deposit"_n,
                            make_tuple(
                                from,
                                quantity,
                                tokencontract,
                                memo
                            )
                        ).send();

                        break;
                    }

                    // Perform a deposit for the user and create an allowance for a third party
                    case name("allowance").value: {

                        name collector = vapaee::utils::check_name_from_string(memo_parts[1]);

                        string memo = "";
                        // if memo_parts.size() > 2 then we have a memo for the allowance
                        if (memo_parts.size() > 2) {
                            memo = memo_parts[2];
                            for (int i=3; i<memo_parts.size(); i++) {
                                memo += " " + memo_parts[i];
                            }
                        }

                        action(
                            permission_level{get_self(), "active"_n},
                            get_self(),
                            "deposit"_n,
                            make_tuple(
                                from,
                                quantity,
                                tokencontract,
                                memo
                            )
                        ).send();

                        action(
                            permission_level{get_self(), "active"_n},
                            get_self(),
                            "allowance"_n,
                            make_tuple(
                                from,
                                collector,
                                quantity
                            )
                        ).send();

                        break;
                    }

                }
                PRINT("vapaee::token::handler::handle_token_transfer()...\n");

            }


        };     
    };
};
