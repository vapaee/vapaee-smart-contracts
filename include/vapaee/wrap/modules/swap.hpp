#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/wrap/errors.hpp>
#include <vapaee/pool/modules/swap.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/market.hpp>


namespace vapaee {
    namespace wrap {

        namespace swap {

            inline name get_self() {
                return vapaee::wrap::contract;
            }

            string aux_replace_all(string str, string from, string to) {
                return vapaee::pool::swap::aux_replace_all(str, from, to);
            }

            string aux_create_swap_memo(string memo_template, map<name, string> variables) {
                return vapaee::pool::swap::aux_create_swap_memo(memo_template, variables);
            }


            void aux_record_conversion(
                    name recipient,
                    name converter,
                    asset price,
                    asset quantity,
                    asset total,
                    asset fee
            ) {
                PRINT("vapaee::wrap::swap::aux_record_conversion()\n");
                PRINT(" recipient: ", recipient.to_string(), "\n");              
                PRINT(" converter: ", converter.to_string(), "\n");              
                PRINT(" price: ", price.to_string(), "\n");              
                PRINT(" quantity: ", quantity.to_string(), "\n");              
                PRINT(" total: ", total.to_string(), "\n");              
                PRINT(" fee: ", fee.to_string(), "\n");

                action(
                    permission_level{get_self(), "active"_n},
                    vapaee::dex::contract,
                    "regpoolswap"_n,
                    std::make_tuple(recipient,converter,price,quantity,total,fee)
                ).send();
            }
            

            /**
            * @brief Convert tokens from one pool to another.
            * 
            * @param quantity Amount of tokens to convert.
            * @param path_str Path is a list of steps separated with spaces: "<account>/<symbol> <account>/<symbol>".
            * @param min_str Minimum amount of tokens to receive.
            * @param recipient_str Final recipient account.
            * @param swap_fee Fee to pay for the conversion.
            */

            void convert(asset quantity, string path_str, string min_str, string recipient_str, string memo, asset swap_fee) {
                PRINT("vapaee::wrap::swap::convert()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" path_str: ", path_str.c_str(), "\n");
                PRINT(" min_str: ", min_str.c_str(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");
                PRINT(" recipient_str: ", recipient_str.c_str(), "\n");

                // general protocol parsing
                asset min = vapaee::utils::check_asset_from_string(min_str);
                name recipient = vapaee::utils::check_name_from_string(recipient_str);
                check(is_account(recipient), create_error_name1(ERROR_SCV_1, recipient).c_str());
                
                // get first element of path and check is not empty
                vector<string> jumps = split(path_str, " ");
                check(jumps.size() > 0, ERR_EMPTY_PATH);
                vector<string> conversion_data = split(jumps.front(), "/");
                check(conversion_data.size() == 2, create_error_string1(ERROR_SCV_2, jumps.front()).c_str());

                // safety check for the converter name and symbol code
                name converter = vapaee::utils::check_name_from_string(conversion_data[0]);
                symbol_code sym_code = vapaee::utils::check_symbol_code_from_string(conversion_data[1]);
                
                // first step of converter must be self
                check(converter == get_self(), create_error_name2(ERROR_SCV_3, converter, get_self()).c_str());

                // --------------------------------------------------
                // TODO: REX 
                asset total = asset(10000, vapaee::utils::REX_TKN_SYMBOL);
                asset rate = vapaee::wrap::rex::get_rex_price();
                asset total_fee = asset(0, quantity.symbol);

                
                if (quantity.symbol == vapaee::utils::SYS_TKN_SYMBOL) {
                    check(sym_code == vapaee::wrap::TLOSW_TKN_SYMBOL.code(), create_error_symcode1(ERROR_SCV_4, sym_code).c_str());

                    // Deposit TLOS to get REX
                    total = vapaee::wrap::rex::deposit_tlos_to_rex(quantity);

                    // issue TLOSW
                    action(
                        permission_level{get_self(), "active"_n},
                        vapaee::token::contract,
                        "issue"_n,
                        std::make_tuple(vapaee::wrap::contract, total, string("TLOSW issued to age oldest loan"))
                    ).send();

                } else if (quantity.symbol == vapaee::wrap::TLOSW_TKN_SYMBOL) {
                    check(sym_code == vapaee::utils::SYS_TKN_SYMBOL.code(), create_error_symcode1(ERROR_SCV_5, sym_code).c_str());

                    asset matured_rex = vapaee::wrap::rex::get_matured_rex();

                    asset actual_withdraw = quantity;

                    if (quantity.amount <= matured_rex.amount) {
                        // we have enought liquidity to convert
                        actual_withdraw = quantity;
                    } else {
                        // we don't have enought matured rex to withdraw
                        actual_withdraw = matured_rex;
                        
                        // calculate remaining TLOSW to convert
                        asset sheduling = quantity - actual_withdraw;
                        
                        // schedule a withdraw for recipient
                        vapaee::wrap::maintenance::do_schedule_withdraw(recipient, sheduling, 0);
                    }

                    // burn liquid REX
                    action(
                        permission_level{get_self(), "active"_n},
                        vapaee::token::contract,
                        "burn"_n,
                        std::make_tuple(actual_withdraw)
                    ).send();
                    
                    // return REX to get TLOS
                    total = vapaee::wrap::rex::withdraw_REX(actual_withdraw);
                    
                } else {
                    check(false, ERROR_SCV_7);
                }
                
                // --------------------------------------------------

                // create de pool if not exist
                uint64_t market_id = vapaee::dex::market::aux_get_canonical_market_id(
                    vapaee::utils::SYS_TKN_SYMBOL.code(),
                    vapaee::wrap::TLOSW_TKN_SYMBOL.code()
                );

                pools pool_markets(vapaee::wrap::contract, vapaee::wrap::contract.value);
                auto pool_it = pool_markets.find(market_id);
                if (pool_it == pool_markets.end()) {
                    vapaee::pool::liquidity::create_pool(market_id);
                }

                // update pool reserves
                pool_it = pool_markets.find(market_id);
                pool_markets.modify(pool_it, get_self(), [&](auto& row) {
                    row.commodity_reserve = asset(10000000, vapaee::utils::SYS_TKN_SYMBOL);;
                    row.currency_reserve = asset(
                        row.commodity_reserve.amount * rate.amount,
                        vapaee::wrap::TLOSW_TKN_SYMBOL
                    );
                    row.fee = swap_fee;
                });

                // record conversion
                aux_record_conversion(
                    recipient,
                    vapaee::wrap::contract,
                    rate,
                    total,
                    quantity,
                    total_fee
                );
                
                // pop first
                jumps.erase(jumps.begin());

                if (jumps.size() == 0) {
                    // last jump of path, send tokens to recipient
                    print("\ntotal: ", total.to_string(), '\n');
                    print("rate:  ", rate.to_string(), '\n');
                    print("total_fee:   ", total_fee.to_string(), '\n');
                    print("min:   ", min.to_string(), '\n');
                    check(total >= min, create_error_asset2(ERROR_C_8, total, min).c_str());
                
                    if (memo == "") {
                        vector<string> memo_parts;
                        memo_parts.push_back(vapaee::utils::OPENPOOL_PROTOCOL_NAME.to_string());
                        memo_parts.push_back(join(jumps, " "));
                        memo_parts.push_back(min.to_string());
                        memo_parts.push_back(recipient.to_string());
                        memo = join(memo_parts, ",");
                    } else {
                        map<name, string> variables = {
                            { name("path"), join(jumps, " ") },
                            { name("min"), min.to_string() },
                            { name("recipient"), recipient.to_string() },
                        };
                        // TODO: add more data to the openpool memo protocol
                        /*
                            in orther to have more variables to export, we need to track intermetiate-steps data
                            for that we need more data in the memo protocol such as the current future path
                            and the proccesed steps results
                        */
                        memo = aux_create_swap_memo(memo, variables);
                    }

                    action(
                        permission_level{get_self(), "active"_n},
                        vapaee::dex::utils::get_contract_for_token(total.symbol.code()),
                        "transfer"_n, 
                        make_tuple(
                            get_self(), recipient, total, memo)
                    ).send();

                    return;
                }

                // still more jumps to go
                vector<string> next_conversion_data = split(jumps.front(), "/");
                check(next_conversion_data.size() == 2, create_error_string1(ERROR_C_6, jumps.front()).c_str());

                name next_converter = vapaee::utils::check_name_from_string(next_conversion_data[0]);
                check(is_account(next_converter), create_error_name1(ERROR_C_7, next_converter).c_str());

                // if (memo == "") {
                vector<string> memo_parts;
                memo_parts.push_back(vapaee::utils::OPENPOOL_PROTOCOL_NAME.to_string());
                memo_parts.push_back(join(jumps, " "));
                memo_parts.push_back(min.to_string());
                memo_parts.push_back(recipient.to_string());
                memo_parts.push_back(memo); // keep last memo
                memo = join(memo_parts, ",");
                //}

                // in case the next jump is towards self, send to echocontract
                if (next_converter == get_self())
                    next_converter = vapaee::echo::contract;

                action(
                    permission_level{get_self(), "active"_n},
                    vapaee::dex::utils::get_contract_for_token(total.symbol.code()),
                    "transfer"_n,
                    make_tuple(
                        get_self(), next_converter, total, memo)
                ).send();

            }


            // init that calls vote_for_producers()
            void action_init() {
                PRINT("vapaee::wrap::swap::action_init()\n");
            }
            
        };     
    };
};
