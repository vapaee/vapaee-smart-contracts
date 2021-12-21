#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pool/utils.hpp>

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
            
            void convert(asset quantity, string path_str, string min_str, string recipient_str, asset conversion_fee) {
                PRINT("vapaee::pool::swap::convert()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" path_str: ", path_str.c_str(), "\n");
                PRINT(" min_str: ", min_str.c_str(), "\n");
                PRINT(" recipient_str: ", recipient_str.c_str(), "\n");

                // general protocol parsing
                asset min = asset_from_string(min_str);
                name recipient = name(recipient_str);
                check(is_account(recipient), ERR_RECIPIENT_NOT_FOUND);
                
                // get first element of path
                vector<string> jumps = split(path_str, " ");
                check(jumps.size() > 0, ERR_EMPTY_PATH);

                vector<string> conversion_data = split(jumps.front(), "/");
                check(conversion_data.size() == 2, ERR_MEMO_PARSING);
                check(name(conversion_data[0]) == get_self(), ERR_INCORRECT_CONVERTER);

                // find pool
                symbol_code A = quantity.symbol.code();
                symbol_code B = symbol_code(conversion_data[1]);

                from pool_markets(get_self(), get_self().value);
                auto sym_index = pool_markets.get_index<"symbols"_n>();
                auto pool_it = sym_index.find(symbols_get_index(A, B));

                if (pool_it == sym_index.end())
                    pool_it = sym_index.find(symbols_get_index(B, A));

                check(pool_it != sym_index.end(), ERR_POOL_NOT_FOUND);

                // check if pool has funds
                check(pool_it->commodity_reserve.amount > 0, ERR_POOL_NOT_FUNDED);
                check(pool_it->currency_reserve.amount > 0, ERR_POOL_NOT_FUNDED);

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
                check(next_conversion_data.size() == 2, ERR_MEMO_PARSING);

                name next_converter = name(next_conversion_data[0]);
                check(is_account(next_converter), ERR_CONVERTER_NOT_FOUND);

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
