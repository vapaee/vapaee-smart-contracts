#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pool/modules/liquidity.hpp>
#include <vapaee/pool/modules/swap.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/pool/errors.hpp>
#include <vapaee/dex/modules/token.hpp>

namespace vapaee {
    namespace pool {
        namespace handler {

            inline name get_self() {
                return vapaee::pool::contract;
            }
            
            void handle_pool_transfer(name from, name to, asset quantity, string memo, name tokencontract) {
                PRINT("vapaee::pool::handler::handle_pool_transfer()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");

                // asset swap_fee = vapaee::dex::global::get().swap_fee;
                // check(swap_fee.symbol.code() == vapaee::utils::FEE_TKN_CODE, create_error_asset1(ERROR_HPT_0, swap_fee).c_str());


                // skip handling some cases
                if (from == get_self() ||  // skip if transaction comes from this contract
                    to != get_self() ||    // skip if contract is not target of transactions
                    memo == "skip") {       // skip if memo is "skip"
                    PRINT("vapaee::pool::handler::handle_pool_transfer()... skipping\n");
                    return;
                }

                // check if token is valid (token is registered, tradeable, genuine and not blacklisted)
                vapaee::dex::security::aux_check_token_ok(quantity.symbol, tokencontract, ERROR_HPT_1);
                
                // parsing memo
                vector<string> memo_tokens = split(memo, ",");
                check(memo_tokens.size() > 0, create_error_string1(ERROR_HPT_2, memo).c_str());

                // safety check if first part of memo is valid
                name header = vapaee::utils::check_name_from_string(memo_tokens[0]);

                switch(header.value) {

                    // investor funding a pool with liquidity
                    case "fund"_n.value: {
                        // memo: "fund,CNT/TLOSV,alice"
                        check(memo_tokens.size() == 2 || memo_tokens.size() == 3, create_error_string1(ERROR_HPT_3, memo).c_str());

                        if (memo_tokens.size() == 3) {
                            from = vapaee::utils::check_name_from_string(memo_tokens[2]);
                        }

                        vapaee::pool::liquidity::fund_attempt(from, quantity, memo_tokens[1]);
                        break;
                    }

                    case "swapfund"_n.value: {

                        // memo: "swapfund,CNT/TLOSV,alice"
                        check(memo_tokens.size() == 2 || memo_tokens.size() == 3, create_error_string1(ERROR_HPT_4, memo).c_str());

                        if (memo_tokens.size() == 3) {
                            from = vapaee::utils::check_name_from_string(memo_tokens[2]);
                        }
                        
                        // calculate optimum swap amount of token A to B, in order to fund pool A/B
                        
                        string market_name = memo_tokens[1];
                        tuple<asset, asset> data = vapaee::pool::swap::calculate_op_swapfund(quantity, market_name);
                        asset _swapfund = std::get<0>(data);
                        asset _swapped  = std::get<1>(data);
                        asset _justfund = quantity - _swapfund;

                        // fund pool A/B
                        string path_str = "telospooldex/" + _swapped.symbol.code().to_string();
                        string swap_memo = string("fund,") + market_name + "," + from.to_string();
                        string recipient_str = vapaee::echo::contract.to_string();
                        string min_str = _swapped.to_string();
                        vapaee::pool::swap::convert(_swapfund, path_str, min_str, recipient_str, swap_memo);
                        vapaee::pool::liquidity::fund_attempt(from, _justfund, memo_tokens[1]);

                        break;
                    }

                    case "profits"_n.value: {
                        // memo: "profits,CNT/TLOSV"
                        check(memo_tokens.size() == 2, create_error_string1(ERROR_HPT_5, memo).c_str());
                        
                        vapaee::pool::liquidity::add_profits(quantity, memo_tokens[1]);

                        break;
                    }

                    // user converts one amount of tokens for another (SWAP)
                    case "openpool.v1"_n.value: {
                        // memo: "openpool.v1,telospooldex/TLOS,0.0000 TLOS,alice,optional memo"
                        check(memo_tokens.size() == 5, create_error_string1(ERROR_HPT_6, memo).c_str());
                        vapaee::pool::swap::convert(quantity, memo_tokens[1], memo_tokens[2], memo_tokens[3], memo_tokens[4]);
                        break;
                    }

                    default: {
                        check(false, create_error_string1(ERROR_HPT_7, memo).c_str());
                        break;
                    }

                }


                PRINT("vapaee::pool::handler::handle_pool_transfer()...\n");

            }

        };     
    };
};
