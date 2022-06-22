#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/wrap/errors.hpp>
#include <vapaee/wrap/modules/lendings.hpp>
#include <vapaee/wrap/modules/investments.hpp>
#include <vapaee/wrap/modules/swap.hpp>


namespace vapaee {
    namespace wrap {
        namespace handler {

            inline name get_self() {
                return vapaee::wrap::contract;
            }

            tuple<uint64_t, name> check_cancel_or_changedebt_memo(asset quantity, const vector<string>& memo_tokens, const string& memo) {
                // parse the second part of the memo to extract the loan id to be cancelled
                check(memo_tokens.size() == 3, create_error_string1(ERROR_CCOCM_1, memo).c_str());
                
                uint64_t loan_id = vapaee::utils::check_float_from_string(memo_tokens[1].c_str());
                name user = vapaee::utils::check_name_from_string(memo_tokens[2].c_str());

                return make_tuple(loan_id, user);
            }

            string check_profits_memo(asset quantity, const vector<string>& memo_tokens, const string& memo) {
                // parse the second part of the memo to extract the proffits params
                check(memo_tokens.size() == 1 || memo_tokens.size() == 2, create_error_string1(ERROR_CPM_1, memo).c_str());
                
                string result = "";
                if (memo_tokens.size() == 2) {
                    result = memo_tokens[1];
                }

                return result;
            }
            
            /*asset check_loan_memo(asset quantity, const vector<string>& memo_tokens, const string& memo) {
                asset telosv = asset(0, vapaee::wrap::TLOSV_TKN_SYMBOL);

                // check if second part of memo is valid.
                check(memo_tokens.size() == 2, create_error_string1(ERROR_CLM_1, memo).c_str());
                string second_part = memo_tokens[1];
                vector<string> second_split = split(second_part, " ");
                asset aux_amount;
                if (second_split.size() == 1) {
                    // this must be a number expressing the percentage of the loan that will be taken in TLOSV.
                    // parse second part into a double and check if it is between 0 and 1
                    double percent = vapaee::utils::check_float_from_string(second_split[0]);
                    check(percent >= 0 && percent <= 1, create_error_double1(ERROR_CLM_2, percent).c_str());

                    // calculate the amount of TLOSV that will be taken out of the loan.
                    // amount is a percentage of the quantity.
                   telosv.amount = quantity.amount * percent;

                } else if (second_split.size() == 2) {
                    // this must be an asset expressing how much TLOSV will be taken in this loan.
                    // parse the second part into an asset and check if it is lower than the quantity
                    aux_amount = vapaee::utils::check_asset_from_string(second_split[1]);
                    check(aux_amount.amount <= quantity.amount, create_error_asset2(ERROR_CLM_3, aux_amount, quantity).c_str());

                    // check if the amount is in TLOSV
                    check(aux_amount.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL, create_error_asset1(ERROR_CLM_4, aux_amount).c_str());

                    telosv.amount = aux_amount.amount;
                } else {
                    check(false, create_error_string1(ERROR_CLM_5, memo).c_str());
                }

                return telosv;
            }*/



            bool handle_possible_swap_conversion(name from, asset quantity, string memo) {
                PRINT("vapaee::wrap::handler::handle_possible_swap_conversion()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");

                // split the memo like a swap conversion
                vector<string> memo_tokens = split(memo, ",");
                check(memo_tokens.size() > 0, create_error_string1(ERROR_HPCS_1, memo).c_str());

                // take the header
                name header = vapaee::utils::check_name_from_string(memo_tokens[0]);
                
                // check if the memo is a swap
                if (vapaee::utils::OPENPOOL_PROTOCOL_NAME.value != header.value) {
                    PRINT("vapaee::wrap::handler::handle_possible_swap_conversion()... not a swap\n");
                    return false;
                }

                asset swap_fee = asset(0, vapaee::utils::FEE_TKN_SYMBOL);
                vapaee::wrap::swap::convert(quantity, memo_tokens[1], memo_tokens[2], memo_tokens[3], memo_tokens[4], swap_fee);

                return true;
            }


            /**
            
            * @brief Processes any transfer entrance to this telos wrapper contract. The parameters are validated and the memo is parsed to execute the correct action. valid memos are: 
            * 
            * 1 - User take out a loan: "loan|<percent>" or "loan|<amount-in-tlosv>"
            * 2 - User cancel a loan: "cancel|<loan-id>" (quantity symbol must be TLOSV)
            * 3 - User redeem token: "redeem" (quantity symbol must be TLOSW)
            * 4 - System get profits: "profits"
            * 
            * @param from The account that is sending tokens.
            * @param to The account that is receiving tokens.
            * @param quantity The quantity of tokens to send.
            * @param memo The memo string containing the action to be performed.
            * @param tokencontract The name of the token contract that is sending tokens.
            
            * @return std::tuple<std::string, std::string> 
            */
            void handle_wrap_transfer(name from, name to, asset quantity, string memo, name tokencontract) {
                PRINT("vapaee::wrap::handler::handle_wrap_transfer()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");

                // skip handling some cases
                if (from == get_self() ||  // skip if transaction comes from this contract
                    to != get_self() ||    // skip if contract is not target of transactions
                    memo == "skip") {       // skip if memo is "skip"
                    PRINT("vapaee::wrap::handler::handle_wrap_transfer()... skipping\n");
                    return;
                }

                // check if token is valid (token is registered, tradeable, genuine and not blacklisted)
                vapaee::dex::security::aux_check_token_ok(quantity.symbol, tokencontract, ERROR_HWT_1);

                // parsing the memo and cheking integrity
                vector<string> memo_tokens = split(memo, "|");
                check(memo_tokens.size() > 0, create_error_string1(ERROR_HWT_2, memo).c_str());

                
                bool was_a_swap = handle_possible_swap_conversion(from, quantity, memo);

                if (was_a_swap) {
                    PRINT("vapaee::wrap::handler::handle_wrap_transfer()... was a swap\n");
                    return;
                }

                // safety check if first part of memo is valid
                name header = vapaee::utils::check_name_from_string(memo_tokens[0]);

                
                switch(header.value) {

                    // Do not proccess the transfer
                    case name("skip").value: {
                        break;
                    }

                    // User take out a loan
                    case name("loan").value: {

                        // check if second part of memo is valid and get telosv quantity
                        // asset telosv_quantity = check_loan_memo(quantity, memo_tokens, memo);

                        // execute the action
                        vapaee::wrap::lendings::handle_loan(from, quantity);
                        break;
                    }

                    /*case name("changedebt").value: {

                        // verify if it is one of the two wrapper tokens (TLOSV or TLOSW)
                        check(
                            quantity.symbol == vapaee::wrap::TLOSV_TKN_SYMBOL ||
                            quantity.symbol == vapaee::wrap::TLOSW_TKN_SYMBOL,
                            create_error_string1(ERROR_HWT_3, quantity.symbol.code().to_string()).c_str()
                        );

                        // parse the second part of the memo to extract the loan id to be cancelled
                        uint64_t loan_id = check_cancel_or_changedebt_memo(quantity, memo_tokens, memo);

                        // execute the action
                        vapaee::wrap::lendings::handle_change_debt(from, quantity, loan_id);
                        break;
                    }*/

                    // User cancel a loan
                    case name("cancel").value: {

                        // parse the second part of the memo to extract the loan id to be cancelled
                        tuple<uint64_t, name> result = check_cancel_or_changedebt_memo(quantity, memo_tokens, memo);
                        uint64_t loan_id = std::get<0>(result);
                        name user = std::get<1>(result);

                        // cancel the user loan
                        vapaee::wrap::lendings::handle_cancel_user_loan(user, quantity, loan_id);

                        break;
                    }

                    // User redeem token
                    case name("redeem").value: {

                        // user can only redeem a token if the quantity is TLOSW
                        check(quantity.symbol == vapaee::wrap::TLOSW_TKN_SYMBOL, create_error_asset1(ERROR_HWT_4, quantity).c_str());

                        // execute the action
                        vapaee::wrap::lendings::handle_redeem(from, quantity);

                        break;
                    }
                    
                    // System get profits
                    case name("profits").value: {
                        check(memo_tokens.size() == 1, create_error_string1(ERROR_HWT_5, memo).c_str());

                        vapaee::wrap::investments::handle_profits(quantity, tokencontract);
                        break;                    

                    }

                    default: {
                        check(false, create_error_string1(ERROR_HWT_6, memo).c_str());   
                    }


                }
                

                
                PRINT("vapaee::wrap::handler::handle_wrap_transfer()...\n");

            }


        };     
    };
};
