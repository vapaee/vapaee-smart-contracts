TABLE fund_history_table {
    uint64_t id;
    time_point date;
    
    name funder;
    asset quantity;

    asset commodity_reserve;
    asset currency_reserve;

    uint64_t primary_key() const { return id; }
    uint64_t by_date_key() const { return (uint64_t) date.sec_since_epoch(); }
    uint64_t by_funder_key() const { return funder.value; }
};

// scope: contract
typedef eosio::multi_index<"fundhistory"_n, fund_history_table,
    indexed_by<"date"_n, const_mem_fun<fund_history_table, uint64_t, &fund_history_table::by_date_key>>,
    indexed_by<"funder"_n, const_mem_fun<fund_history_table, uint64_t, &fund_history_table::by_funder_key>>
> fundhistory;

