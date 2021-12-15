// --- events (users events log) -----------
// Everyevent that happens in the dex will be logged in this table.

// scope: contract
// row: represent an event that happened in the DEX DAO system

TABLE events_table {
    uint64_t id;
    name user;
    name event;
    string params;
    time_point_sec date;
    uint64_t primary_key() const { return id; }
    uint64_t by_user_key() const { return user.value; }
    uint64_t by_date_key() const { return (uint64_t) date.sec_since_epoch(); }
};

typedef eosio::multi_index< "events"_n, events_table,
    indexed_by<"date"_n, const_mem_fun<events_table, uint64_t, &events_table::by_date_key>>,
    indexed_by<"user"_n, const_mem_fun<events_table, uint64_t, &events_table::by_user_key>>
> events;
