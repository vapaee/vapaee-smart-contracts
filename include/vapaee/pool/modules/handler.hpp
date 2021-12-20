#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pool/modules/liquidity.hpp>
#include <vapaee/pool/modules/swap.hpp>

namespace vapaee {
    namespace pool {
        namespace handler {

            inline name get_self() {
                return vapaee::pool::contract;
            }

            
            void handler_transfer(name from, name to, asset quantity, string memo, name tokencontract, asset conversion_fee) {
                PRINT("vapaee::pool::handler::handler_transfer()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");

                // skip handling some cases
                if (from == get_self() ||  // skip if transaction comes from this contract
                    to != get_self() ||    // skip if contract is not target of transactions
                    memo == "skip")        // skip if memo is "skip"
                    PRINT("vapaee::pool::handler::handler_transfer()... skipping\n");
                    return;

                // parsing memo
                vector<string> memo_tokens = split(memo, ",");
                check(memo_tokens.size() > 0, ERR_MEMO_PARSING);


                // ----------------------------------------------------
                // TODO: check if token is valid (using tokencontract)
                // ----------------------------------------------------
                
                switch(name(memo_tokens[0]).value) {

                    case "fund"_n.value :
                        check(memo_tokens.size() == 2, ERR_MEMO_PARSING);
                        vapaee::pool::liquidity::fund(from, quantity, memo_tokens[1]);
                        break;
                    
                    case PROTO_VERSION.value:
                        check(memo_tokens.size() == 4, ERR_MEMO_PARSING);
                        vapaee::pool::swap::convert(quantity, memo_tokens[1], memo_tokens[2], memo_tokens[3], conversion_fee);
                        break;

                }


                PRINT("vapaee::pool::handler::handler_transfer()...\n");

            }

        };     
    };
};
