#include "./_aux.hpp"

        // TABLE historyall -----------
        // scope: contract
        TABLE historyall_table {
            uint64_t id;       // auxiliar own id
            uint64_t key;      // key of the entry in history table
            uint64_t market;   // market-id used as scope for the history table
            time_point_sec date;
            uint64_t primary_key() const { return id; }
            uint64_t by_marketkey_key() const { return (market<<50) + key; }
            // uint64_t by_key_key() const { return key; }
            // uint64_t by_market_key() const { return market; }
        };
        typedef eosio::multi_index< "historyall"_n, historyall_table,
            indexed_by<"marketkey"_n, const_mem_fun<historyall_table, uint64_t, &historyall_table::by_marketkey_key>>> historyall;
        // ------------------------------------