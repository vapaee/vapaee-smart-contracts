// --- historyall ---
// For maintenance purposes, this table register every history entry for every market.
// That way it's easier o sort them by data

// scope: contract
// row: represents a reference to a market history entry

TABLE historyall_table {
    uint64_t id;       // auxiliar own id
    uint64_t key;      // key of the entry in history table
    uint64_t market;   // market-id used as scope for the history table
    time_point_sec date;
    uint64_t primary_key() const { return id; }
    uint64_t by_marketkey_key() const { return (market<<50) + key; }
};

typedef eosio::multi_index< "historyall"_n, historyall_table,
    indexed_by<"marketkey"_n, const_mem_fun<historyall_table, uint64_t, &historyall_table::by_marketkey_key>>
> historyall;
    