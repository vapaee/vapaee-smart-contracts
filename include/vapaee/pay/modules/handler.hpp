#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pay/errors.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <vapaee/pay/modules/deposit.hpp>

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


            void handle_pay_transfer(name from, name to, asset quantity, string memo, name paycontract) {
                PRINT("vapaee::pay::handler::handle_pay_transfer()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");

                // skip handling some cases
                if (from == get_self() ||  // skip if transaction comes from this contract
                    to != get_self() ||    // skip if contract is not target of transactions
                    memo == "skip") {       // skip if memo is "skip"
                    PRINT("vapaee::pay::handler::handle_pay_transfer()... skipping\n");
                    return;
                }

                // check if pay is valid (pay is registered, tradeable, genuine and not blacklisted)
                vapaee::dex::security::aux_check_token_ok(quantity.symbol, paycontract, ERROR_HPT_1);

                // parsing the memo and cheking integrity
                vector<string> memo_pays = split(memo, "|");
                check(memo_pays.size() > 0, create_error_string1(ERROR_HPT_2, memo).c_str());

                // safety check if first part of memo is valid
                name header = vapaee::utils::check_name_from_string(memo_pays[0]);

                
                switch(header.value) {

                    // Do not proccess the transfer
                    case name("skip").value: {
                        break;
                    }

                    // Perform deposit
                    case name("deposit").value: {

                        // check if memo has another parameter
                        name receiver = check_deposit_memo(from, memo_pays, memo);

                        // execute the action
                        vapaee::pay::deposit::perform_deposit(receiver, quantity);

                        break;
                    }

                    // System get profits
                    case name("profits").value: {
                        // check(memo_tokens.size() == 1, create_error_string1(ERROR_HPT_5, memo).c_str());

                        // vapaee::pay::investments::handle_profits(quantity, tokencontract);
                        break;                    

                    }

                    default: {
                        check(false, create_error_string1(ERROR_HPT_6, memo).c_str());   
                    }


                }
                

                
                PRINT("vapaee::pay::handler::handle_pay_transfer()...\n");

            }


        };     
    };
};