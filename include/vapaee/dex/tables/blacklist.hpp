#include "./_aux.hpp"

        // TABLE blacklist -----------
        // scope: contract
        TABLE blacklist_table {
            uint64_t id;
            symbol_code symbol;
            uint8_t precision;
            name contract;
            name ballot;
            uint64_t primary_key() const { return id; }
            uint64_t by_symbol_key() const { return symbol.raw(); }
        };
        typedef eosio::multi_index< "blacklist"_n, blacklist_table,
            indexed_by<"symbol"_n, const_mem_fun<blacklist_table, uint64_t, &blacklist_table::by_symbol_key>>
        > blacklist;
        // ------------------------------------
