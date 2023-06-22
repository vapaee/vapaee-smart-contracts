#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pool/utils.hpp>
#include <vapaee/pool/errors.hpp>
#include <vapaee/dex/modules/utils.hpp>

namespace vapaee {
    namespace pool {
        namespace utils {
            asset get_pool_rate(uint64_t pool_id, name converter);
            asset get_pool_price(uint64_t pool_id, name converter);
            tuple<asset, asset, asset, asset> get_conversion(uint64_t pool_id, asset quantity);
            uint64_t extract_canonical_market_id_from_market_name(string market_name);
        };
    };
};

namespace vapaee {
    namespace dex {
        namespace utils {
            name get_contract_for_token(symbol_code token_code);
        };
    };
};

namespace vapaee {
    namespace pool {

        namespace swap {

            inline name get_self() {
                return vapaee::pool::contract;
            }

            string aux_replace_all(string str, string from, string to) {
                size_t start_pos = 0;
                while((start_pos = str.find(from, start_pos)) != std::string::npos) {
                    str.replace(start_pos, from.length(), to);
                    start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
                }
                return str;
            }

            string aux_create_swap_memo(string memo_template, map<name, string> variables) {
                // init memo with template
                string memo = memo_template;

                // for every variable, replace it in the memo using aux_replace_all(memo, "{" + to_string(i) + "}", replacement)
                for(auto& variable : variables) {
                    string replacement = variable.second;
                    memo = aux_replace_all(memo, "{" + variable.first.to_string() + "}", replacement);
                }
                
                return memo;
            }

            asset get_pool_rate(uint64_t market_id, name converter) {
                // vapaee::pool::swap::get_pool_rate()
                return vapaee::pool::utils::get_pool_rate(market_id, converter);
            }


            asset get_pool_price(uint64_t market_id, name converter) {
                // vapaee::pool::swap::get_pool_price()
                return vapaee::pool::utils::get_pool_price(market_id, converter);
            }
            
            void aux_record_conversion(
                    name recipient,
                    name converter,
                    asset price,
                    asset sent,
                    asset result,
                    asset fee
            ) {
                action(
                    permission_level{get_self(), "active"_n},
                    vapaee::dex::contract,
                    "regpoolswap"_n,
                    std::make_tuple(recipient,converter,price,sent,result,fee)
                ).send();
            }
            

            /**
            * @brief Convert tokens from one pool to another.
            * 
            * @param quantity Amount of tokens to convert.
            * @param path_str Path is a list of steps separated with spaces: "<account>/<symbol> <account>/<symbol>".
            * @param min_str Minimum amount of tokens to receive.
            * @param recipient_str Final recipient account.
            */

            void convert(asset quantity, string path_str, string min_str, string recipient_str, string memo) {
                PRINT("vapaee::pool::swap::convert()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" path_str: ", path_str.c_str(), "\n");
                PRINT(" min_str: ", min_str.c_str(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");
                PRINT(" recipient_str: ", recipient_str.c_str(), "\n");

                // general protocol parsing
                asset min = vapaee::utils::check_asset_from_string(min_str);
                name recipient = vapaee::utils::check_name_from_string(recipient_str);
                check(is_account(recipient), create_error_name1(ERROR_C_1, recipient).c_str());

                // get first element of path and check is not empty
                vector<string> jumps = vapaee::utils::split(path_str, " ");
                check(jumps.size() > 0, ERR_EMPTY_PATH);
                vector<string> conversion_data = vapaee::utils::split(jumps.front(), "/");
                check(conversion_data.size() == 2, create_error_string1(ERROR_C_2, jumps.front()).c_str());

                // safety check for the converter name and symbol code
                name converter = vapaee::utils::check_name_from_string(conversion_data[0]);
                symbol_code sym_code = vapaee::utils::check_symbol_code_from_string(conversion_data[1]);

                // first step of converter must be self
                check(converter == get_self(), create_error_name2(ERROR_C_3, converter, get_self()).c_str());

                // --------------------------------------------------

                // find pool
                symbol_code A = quantity.symbol.code();
                symbol_code B = sym_code;
                pools pool_markets(get_self(), get_self().value);
                auto sym_index = pool_markets.get_index<"symbols"_n>();
                auto pool_it = sym_index.find(pack_symbols_in_uint128(A, B));

                // if not found try inverse
                if (pool_it == sym_index.end())
                    pool_it = sym_index.find(pack_symbols_in_uint128(B, A));

                // check pool exists
                check(pool_it != sym_index.end(),
                    create_error_string3(
                        ERROR_C_9,
                        A.to_string(),
                        B.to_string(),
                        get_self().to_string()
                    ).c_str()
                );

                // check if pool has funds
                check(pool_it->commodity_reserve.amount > 0, create_error_symbol1(ERROR_C_4, pool_it->commodity_reserve.symbol).c_str());
                check(pool_it->currency_reserve.amount > 0, create_error_symbol1(ERROR_C_5, pool_it->currency_reserve.symbol).c_str());

                // make conversion
                tuple<asset, asset, asset, asset> result = vapaee::pool::utils::get_conversion(pool_it->market_id, quantity);
                asset total = std::get<0>(result);
                asset rate = std::get<1>(result);
                asset price = std::get<2>(result);
                asset total_fee = std::get<3>(result);

                // update pool reserves
                sym_index.modify(pool_it, get_self(), [&](auto& row) {
                    if (quantity.symbol.code() == row.commodity_reserve.symbol.code()) {
                        row.commodity_reserve += quantity;
                        row.currency_reserve -= total;
                    } else {
                        row.currency_reserve += quantity;
                        row.commodity_reserve -= total;
                    }
                });

                // record conversion
                aux_record_conversion(
                    recipient,
                    get_self(),
                    price,
                    quantity,
                    total,
                    total_fee
                );
                
                // pop first
                jumps.erase(jumps.begin());


                //if (jumps.size() == 2) check(false, create_error_asset1("JUMPS 2 - total:", total).c_str());

                if (jumps.size() == 0) {
                    // last jump of path, send tokens to recipient
                    print("\ntotal: ", total.to_string(), '\n');
                    print("rate:  ", rate.to_string(), '\n');
                    print("total_fee:   ", total_fee.to_string(), '\n');
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

                    // in case recipient is self, then auto-send a selftransf instead of normal transfer
                    if (recipient == get_self()) {
                        // send a local action to simulate a transfer to self
                        action(
                            permission_level{get_self(), "active"_n},
                            get_self(),
                            "selftransf"_n,
                            make_tuple(
                                get_self(), recipient, total, memo)
                        ).send();
                    } else {
                        // send a real transfer to the recipient
                        action(
                            permission_level{get_self(), "active"_n},
                            vapaee::dex::utils::get_contract_for_token(total.symbol.code()),
                            "transfer"_n, 
                            make_tuple(
                                get_self(), recipient, total, memo)
                        ).send();
                    }

                    return;
                }

                // still more jumps to go
                vector<string> next_conversion_data = vapaee::utils::split(jumps.front(), "/");
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

                // in case the next jump is towards self, auto send a selftransf instead
                if (next_converter == get_self()) {
                    // send a local action to simulate a transfer to self
                    action(
                        permission_level{get_self(), "active"_n},
                        get_self(),
                        "selftransf"_n,
                        make_tuple(
                            get_self(), next_converter, total, memo)
                    ).send();                    
                } else {
                    // send a real transfer to the next converter to process the next step
                    action(
                        permission_level{get_self(), "active"_n},
                        vapaee::dex::utils::get_contract_for_token(total.symbol.code()),
                        "transfer"_n,
                        make_tuple(
                            get_self(), next_converter, total, memo)
                    ).send();
                }

                PRINT("vapaee::pool::swap::convert() ...\n");
            }

            tuple<asset, asset> calculate_op_swapfund(asset quantity, string market_name) {
                PRINT("vapaee::pool::swap::calculate_op_swapfund()\n");
                PRINT("quantity: ", quantity.to_string(), '\n');
                PRINT("market_name: ", market_name, '\n');
                

                // find pool
                uint64_t market_id = vapaee::pool::utils::extract_canonical_market_id_from_market_name(market_name);
                pools pool_markets(get_self(), get_self().value);
                auto pool_it = pool_markets.find(market_id);

                //--------------------------------------
                asset from_reserve, to_reserve;
                asset swap_fee = pool_it->fee;
                if (quantity.symbol.code() == 
                        pool_it->commodity_reserve.symbol.code()) {
                    from_reserve = pool_it->commodity_reserve;
                    to_reserve = pool_it->currency_reserve;
                } else {
                    from_reserve = pool_it->currency_reserve;
                    to_reserve = pool_it->commodity_reserve;
                }
                
                asset from_reserve_ex = vapaee::utils::asset_change_precision(from_reserve, ARITHMETIC_PRECISION);
                asset to_reserve_ex = vapaee::utils::asset_change_precision(to_reserve, ARITHMETIC_PRECISION);
                asset quantity_ex = vapaee::utils::asset_change_precision(quantity, ARITHMETIC_PRECISION);                
                
                PRINT("swap_fee: ", swap_fee.to_string(), '\n');
                PRINT("from_reserve_ex: ", from_reserve_ex.to_string(), '\n');
                PRINT("to_reserve_ex: ", to_reserve_ex.to_string(), '\n');
                PRINT("quantity_ex: ", quantity_ex.to_string(), '\n');
               

                asset first_part_ex = asset(quantity_ex.amount/2, quantity_ex.symbol);
                asset second_part_ex = asset(quantity_ex.amount-first_part_ex.amount, quantity_ex.symbol);
                asset fee_ex = asset(0, quantity_ex.symbol);
                asset rate, swapped_ex, swapped, final_rate, fund_rate, final_inv, fund_inv;

                for (int i=0; i<5; i++) {
                    // tuple<asset, asset, asset> result = vapaee::pool::utils::get_conversion(market_id, second_part_ex);
                    fee_ex = asset_multiply(swap_fee, second_part_ex);
                    first_part_ex.amount = quantity_ex.amount - second_part_ex.amount;
                    rate = asset_divide(to_reserve_ex, from_reserve_ex + second_part_ex);
                    swapped_ex = asset_multiply(second_part_ex - fee_ex, rate);
                    swapped = asset_change_precision(swapped_ex, to_reserve.symbol.precision());

                    fund_rate = asset_divide(swapped_ex, first_part_ex);
                    final_rate = asset_divide(to_reserve_ex - swapped_ex, from_reserve_ex + second_part_ex);

                    PRINT(" --> fund_rate:    ", fund_rate.to_string(), '\n');
                    PRINT(" --> final_rate:   ", final_rate.to_string(), '\n');

                    final_inv = inverse(final_rate, from_reserve_ex.symbol);
                    fund_inv = inverse(fund_rate, from_reserve_ex.symbol);

                    if (fund_rate.amount > final_rate.amount) {
                        asset diff = asset_multiply(swapped_ex, final_inv) - asset_multiply(swapped_ex, fund_inv);
                        PRINT(" --> ", asset_multiply(swapped_ex, final_inv).to_string(), " - ", asset_multiply(swapped_ex, fund_inv).to_string(), " = ", diff.to_string(), '\n');
                        second_part_ex.amount -= diff.amount * 0.5;
                        PRINT(" --------------> ", second_part_ex.to_string(), '\n');
                    } else {
                        asset diff = asset_multiply(swapped_ex, fund_inv) - asset_multiply(swapped_ex, final_inv);
                        PRINT(" --> ", asset_multiply(swapped_ex, fund_inv).to_string(), " - " , asset_multiply(swapped_ex, final_inv).to_string(), " = ", diff.to_string(), '\n');
                        second_part_ex.amount += diff.amount * 0.5;
                        PRINT(" --------------> ", second_part_ex.to_string(), '\n');
                    }
                }

                rate = asset_divide(to_reserve_ex, from_reserve_ex + second_part_ex);
                swapped_ex = asset_multiply(second_part_ex - fee_ex, rate);
                swapped = asset_change_precision(swapped_ex, to_reserve.symbol.precision());
                asset forswapping = asset_change_precision(second_part_ex, quantity.symbol.precision());
                PRINT(" --> forswapping: ", forswapping.to_string(), '\n');
                PRINT(" --> swapped: ", swapped.to_string(), '\n');

                // This is a patch. This method is used to calculate the best swap for a swapfund.
                // This minimun adjustment in the return value is to avoid dealing with minimun requirement missing by this little in next swap
                swapped.amount -= 2;

                return make_tuple(forswapping, swapped);
            }


            // This function extract assets from a pool and convert them to a new symbol.
            // The new symbol is the symbol of the pool.
            void extract_canonical_market_id_from_market_name(string market_name, uint64_t& market_id) {
                PRINT("vapaee::pool::swap::extract_canonical_market_id_from_market_name()\n");
                PRINT("market_name: ", market_name, '\n');
                PRINT("market_id: ", market_id, '\n');
                
                market_id = vapaee::pool::utils::extract_canonical_market_id_from_market_name(market_name);
                PRINT("market_id: ", market_id, '\n');
            }

            
        }; 
    };
};
