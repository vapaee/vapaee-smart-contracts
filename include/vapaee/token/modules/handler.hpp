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

                // skip handling some cases
                if (from == get_self() ||  // skip if transaction comes from this contract
                    to != get_self() ||    // skip if contract is not target of transactions
                    memo == "skip") {       // skip if memo is "skip"
                    PRINT("vapaee::token::handler::handle_token_transfer()... skipping\n");
                    return;
                }

                // check if token is valid (token is registered, tradeable, genuine and not blacklisted)
                vapaee::dex::security::aux_check_token_ok(quantity.symbol, tokencontract, ERROR_HTT_1);

                // parsing the memo and cheking integrity
                vector<string> memo_tokens = split(memo, "|");
                check(memo_tokens.size() > 0, create_error_string1(ERROR_HTT_2, memo).c_str());

                // safety check if first part of memo is valid
                name header = vapaee::utils::check_name_from_string(memo_tokens[0]);

                
                switch(header.value) {

                    // Do not proccess the transfer
                    case name("skip").value: {
                        break;
                    }

                    default: {
                        check(false, create_error_string1(ERROR_HTT_6, memo).c_str());   
                    }


                }
                

                
                PRINT("vapaee::token::handler::handle_token_transfer()...\n");

            }


        };     
    };
};
