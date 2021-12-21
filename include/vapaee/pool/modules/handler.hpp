#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pool/modules/liquidity.hpp>
#include <vapaee/pool/modules/swap.hpp>
#include <vapaee/dex/modules/token.hpp>

namespace vapaee {
    namespace pool {
        namespace handler {

            inline name get_self() {
                return vapaee::pool::contract;
            }

            
            void handle_pool_transfer(name from, name to, asset quantity, string memo, name tokencontract, asset conversion_fee) {
                PRINT("vapaee::pool::handler::handle_pool_transfer()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");

                // skip handling some cases
                if (from == get_self() ||  // skip if transaction comes from this contract
                    to != get_self() ||    // skip if contract is not target of transactions
                    memo == "skip")        // skip if memo is "skip"
                    PRINT("vapaee::pool::handler::handle_pool_transfer()... skipping\n");
                    return;

                // check if token is valid (token is registered, tradeable, genuine and not blacklisted)
                vapaee::dex::security::aux_check_token_ok(quantity.symbol, tokencontract, ERROR_HPT_1);
                
                // parsing memo
                vector<string> memo_tokens = split(memo, ",");
                check(memo_tokens.size() > 0, ERR_MEMO_PARSING);
                
                switch(name(memo_tokens[0]).value) {

                    // investor funding a pool with liquidity
                    case "fund"_n.value:
                        // memo: "fund,CNT/TLOS"
                        check(memo_tokens.size() == 2, ERR_MEMO_PARSING);
                        vapaee::pool::liquidity::fund(from, quantity, memo_tokens[1]);
                        break;
                    
                    // user converts one amount of tokens for another (SWAP)
                    case PROTO_VERSION.value:
                        // memo: "openpool.v1,telospooldex/TLOS,0.0000 TLOS,alice"
                        check(memo_tokens.size() == 4, ERR_MEMO_PARSING);
                        vapaee::pool::swap::convert(quantity, memo_tokens[1], memo_tokens[2], memo_tokens[3], conversion_fee);
                        break;

                }


                PRINT("vapaee::pool::handler::handle_pool_transfer()...\n");

            }

        };     
    };
};
