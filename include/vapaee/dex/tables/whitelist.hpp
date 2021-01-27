#include "./_aux.hpp"

        // TABLE whitelist -----------
        // scope: contract
        TABLE whitelist_table {
            symbol_code symbol;
            name contract;
            name ballot;
            uint64_t primary_key() const { return symbol.raw(); }
        };
        typedef eosio::multi_index< "whitelist"_n, whitelist_table> whitelist;
        // ------------------------------------
