#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pool/utils.hpp>
#include <vapaee/pool/errors.hpp>

using vapaee::pool::utils::get_conversion;
using vapaee::pool::utils::record_conversion;
using vapaee::utils::split;

namespace vapaee {
    namespace pool {

        using namespace global;

        namespace swap {

            inline name get_self() {
                return vapaee::pool::contract;
            }

            /**
            * @brief Convert tokens from one pool to another.
            * 
            * @param quantity Amount of tokens to convert.
            * @param path_str Path is a list of steps separated with spaces: "<account>/<symbol> <account>/<symbol>".
            * @param min_str Minimum amount of tokens to receive.
            * @param recipient_str Final recipient account.
            * @param conversion_fee Fee to pay for the conversion.
            */

            void convert(asset quantity, string path_str, string min_str, string recipient_str, asset conversion_fee) {
                PRINT("vapaee::pool::swap::convert()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" path_str: ", path_str.c_str(), "\n");
                PRINT(" min_str: ", min_str.c_str(), "\n");
                PRINT(" recipient_str: ", recipient_str.c_str(), "\n");

                // general protocol parsing
                asset min = asset_from_string(min_str);
                name recipient = name(recipient_str);
                check(is_account(recipient), create_error_name1(ERROR_C_1, recipient).c_str());
                
                // get first element of path and check is not empty
                vector<string> jumps = split(path_str, " ");
                check(jumps.size() > 0, ERR_EMPTY_PATH);
                vector<string> conversion_data = split(jumps.front(), "/");
                check(conversion_data.size() == 2, create_error_string1(ERROR_C_2, jumps.front()).c_str());

                // safety check for the converter name and symbol code
                name converter = vapaee::utils::check_name_from_string(conversion_data[0]);
                symbol_code sym_code = vapaee::utils::check_symbol_code_from_string(conversion_data[1]);
                
                // first step of converter must be self
                check(converter == get_self(), create_error_name1(ERROR_C_3, converter).c_str());

                // find pool
                symbol_code A = quantity.symbol.code();
                symbol_code B = sym_code;

                pools pool_markets(get_self(), get_self().value);
                auto sym_index = pool_markets.get_index<"symbols"_n>();
                auto pool_it = sym_index.find(symbols_get_index(A, B));

                if (pool_it == sym_index.end())
                    pool_it = sym_index.find(symbols_get_index(B, A));

                check(pool_it != sym_index.end(), ERR_POOL_NOT_FOUND);

                // check if pool has funds
                check(pool_it->commodity_reserve.amount > 0, create_error_symbol1(ERROR_C_4, pool_it->commodity_reserve.symbol).c_str());
                check(pool_it->currency_reserve.amount > 0, create_error_symbol1(ERROR_C_5, pool_it->currency_reserve.symbol).c_str());

                asset fee = asset_multiply(
                    conversion_fee,
                    asset_change_precision(quantity, ARITHMETIC_PRECISION));

                // make conversion
                tuple<asset, asset> result = get_conversion(pool_it->id, quantity, fee);
                asset total = std::get<0>(result);
                asset rate = std::get<1>(result);
    
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

                // pop first
                jumps.erase(jumps.begin());

                // ------------------------------------------------
                // TODO: record conversion
                // ------------------------------------------------

                if (jumps.size() == 0) {
                    // last jump of path, send tokens to recipient
                    print("\ntotal: ", total, '\n');
                    print("rate:  ", rate, '\n');
                    print("fee:   ", fee, '\n');
                    check(total >= min, ERR_BAD_DEAL);
                
                    // TODO: final transaction memo
                    action(
                        permission_level{get_self(), "active"_n},
                        get_contract_for_token(total.symbol.code()),
                        "transfer"_n, 
                        make_tuple(
                            get_self(), recipient, total, string(""))
                    ).send();
                    return;
                }

                // still more jumps to go
                vector<string> next_conversion_data = split(jumps.front(), "/");
                check(next_conversion_data.size() == 2, create_error_string1(ERROR_C_6, jumps.front()).c_str());

                name next_converter = vapaee::utils::check_name_from_string(next_conversion_data[0]);
                check(is_account(next_converter), create_error_name1(ERROR_C_7, next_converter).c_str());

                vector<string> memo_parts;
                memo_parts.push_back(PROTO_VERSION.to_string());
                memo_parts.push_back(join(jumps, " "));
                memo_parts.push_back(min.to_string());
                memo_parts.push_back(recipient.to_string());

                // in case the next jump is towards self, send to echocontract
                if (next_converter == get_self())
                    next_converter = ECHO_CONTRACT;

                action(
                    permission_level{get_self(), "active"_n},
                    get_contract_for_token(total.symbol.code()),
                    "transfer"_n,
                    make_tuple(
                        get_self(), next_converter, total, join(memo_parts, ","))
                ).send();

                PRINT("vapaee::pool::swap::convert() ...\n");
            }
            
        };     
    };
};
