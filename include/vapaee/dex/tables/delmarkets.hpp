#include "./_aux.hpp"

        // TABLE delmarkets -----------
        // scope: contract
        // list of the markets marked to delete (slowly cancel orders and delete history)
        TABLE delmarkets_table {
            uint64_t id;            // id del market
            name table;             // xxx.yyy
            uint64_t primary_key() const { return id; }
            uint64_t by_table_key() const { return table.value; }
        };
        typedef eosio::multi_index< "delmarkets"_n, delmarkets_table,
            indexed_by<"table"_n, const_mem_fun<delmarkets_table, uint64_t, &delmarkets_table::by_table_key>>
        > delmarkets;
        // ------------------------------------
