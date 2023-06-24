#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>
/*

// --- swaps ---
// Ths is a temporary table to hold in-proccess swaps.

// scope: contract
// row: represent the last of in-proccess swap of a trader

TABLE swaps_table {
    uint64_t id;
    name trader;       // user who swaps
    asset in;          // amount of incoming tokens 
    asset out;         // amount of minimun tolerated outgoing tokens 
    asset got;         // amount of final outgoing tokens

    // list of tuples of trades (asset, asset)
    vector<std::tuple<asset, asset>> trades;


    uint64_t primary_key()const { return id; }
};

typedef eosio::multi_index< "swaps"_n, swaps_table > swaps;

*/

namespace vapaee {
    namespace pool {
        namespace util {
            void send_swap(
                const name& from,
                const asset& quantity,
                const symbol_code& token_to_receive,
                const name& recipiant,
                const string& memo,
                const name& converter
            );
        };
    };
};

namespace vapaee {
    namespace dex {

        using namespace utils;

        namespace swap {

            inline name get_self() {
                return vapaee::dex::contract;
            }

            name aux_get_any_conveter_for_market_id(uint64_t market_id) {
                PRINT("vapaee::dex::market::aux_get_any_conveter_for_market_id()\n");
                PRINT(" market_id: ", std::to_string((long)market_id)," \n");

                converters ctable(get_self(), get_self().value);
                auto market_index = ctable.get_index<"market"_n>();
                auto itr = market_index.find(market_id);
                check(itr != market_index.end(), create_error_id1(ERROR_GACF_1, market_id).c_str());
                
                return itr->converter;
            }

            void handle_start_swap_transfer(name from, name to, asset quantity, string memo, name tokencontract) {
                PRINT("vapaee::dex::swap::handle_start_swap_transfer()\n");
                
                // check if token is valid (token is registered, tradeable, genuine and not blacklisted)
                vapaee::dex::security::aux_check_token_ok(quantity.symbol, tokencontract, ERROR_HSST_1);

                // memo format: "openpool.v1";token;recipiant;swapmemo;
                vector<string> parts = split(memo, ';');
                check(parts.size() == 4, create_error_string1(ERROR_HSST_2, memo).c_str());

                symbol_code token = vapaee::utils::check_symbol_code_from_string(parts[1]);
                name recipiant = vapaee::utils::check_name_from_string(parts[2]);
                string swapmemo = parts[3];

                uint64_t market = vapaee::dex::market::aux_get_market_id(quantity.symbol.code(), token);
                name converter = aux_get_any_conveter_for_market_id(market);

// check(false,
//     (from.to_string() + " " + quantity.to_string() + " " + token.to_string() + " " + recipiant.to_string() + " " + swapmemo + " " + converter.to_string()).c_str()
// );

                vapaee::pool::util::send_swap(
                    vapaee::current_contract,
                    quantity,
                    token,
                    recipiant,
                    swapmemo,
                    converter
                );

                PRINT("vapaee::dex::swap::handle_start_swap_transfer() ...\n");
            }

            // --------------------------------------------------------------

            
        };     
    };
};

#include <vapaee/pool/modules/util.hpp>