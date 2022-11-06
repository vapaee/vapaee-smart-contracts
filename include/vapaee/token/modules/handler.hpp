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

                // skip handling transfers to third party
                check(memo == std::string("deposit"), "If you want to deposit tokens add the word 'deposit' in the memo, otherwise add 'skip' to skip handler");

                action(
                    permission_level{get_self(), "active"_n},
                    get_self(),
                    "deposit"_n,
                    make_tuple(
                        from,
                        quantity,
                        tokencontract
                    )
                ).send();
                
                PRINT("vapaee::token::handler::handle_token_transfer()...\n");

            }


        };     
    };
};
