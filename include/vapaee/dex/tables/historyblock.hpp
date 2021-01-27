#include "./_aux.hpp"

        // TABLE historyblock -----------
        // scope: market-id
        TABLE historyblock_table {
            uint64_t id;
            uint64_t hour;
            asset price;    // current price for this hour (and last)
            asset inverse;
            asset entrance; // first price for this hour
            asset max;      // max price for this hour
            asset min;      // min price for this hour
            asset volume;
            asset amount;
            time_point_sec date;
            uint64_t primary_key() const { return id; }
            uint64_t by_hour_key() const { return hour; }
        };
        typedef eosio::multi_index< "historyblock"_n, historyblock_table,
            indexed_by<"hour"_n, const_mem_fun<historyblock_table, uint64_t, &historyblock_table::by_hour_key>>
        > historyblock;
        // ------------------------------------
