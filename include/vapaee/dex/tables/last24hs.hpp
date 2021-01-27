#include "./_aux.hpp"

        // TABLE last24hs -----------
        // scope: market-id
        TABLE last24hs_table {
            name label;     // "lastone", "h.zero", "h.one", ..., "h.twentythree"
            asset price;    // current price for this hour (and last)
            asset inverse;
            asset entrance; // first price for this hour
            asset max;      // max price for this hour
            asset min;      // min price for this hour            
            asset volume;
            asset amount;
            uint64_t hour;
            time_point_sec date;
            uint64_t primary_key() const { return label.value; }
        };
        typedef eosio::multi_index< "last24hs"_n, last24hs_table > last24hs;
        // ------------------------------------
