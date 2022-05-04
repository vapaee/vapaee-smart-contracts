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


// TODO: do we need this method?


namespace vapaee {
    namespace dex {

        using namespace utils;

        namespace swap {

            void handle_start_swap_transfer(name from, name to, asset quantity, string memo, name tokencontract) {
                PRINT("vapaee::dex::swap::handle_start_swap_transfer()\n");
                
                // check if token is valid (token is registered, tradeable, genuine and not blacklisted)
                vapaee::dex::security::aux_check_token_ok(quantity.symbol, tokencontract, ERROR_HSST_1);

                check(false, "NOT IMPLEMENTED YET");

                PRINT("vapaee::dex::swap::handle_start_swap_transfer() ...\n");
            }

            // --------------------------------------------------------------

            
        };     
    };
};
