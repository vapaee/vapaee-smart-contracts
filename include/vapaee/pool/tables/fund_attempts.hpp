struct [[eosio::table]] funding_attempts_table {
    uint64_t id;
    uint64_t pool_id;
    name funder;
    asset commodity;
    asset currency;
    
    uint64_t primary_key() const { return id; }
    uint64_t by_pool_id() const { return pool_id; }
    uint64_t by_funder() const { return funder.value; }
    uint64_t by_commodity() const { return commodity.symbol.raw(); }
    uint64_t by_currency() const { return currency.symbol.raw(); }
};

// scope: contract.value
typedef eosio::multi_index<"fundattempts"_n, funding_attempts_table,
    indexed_by<"pool"_n, const_mem_fun<funding_attempts_table, uint64_t, &funding_attempts_table::by_pool_id>>,
    indexed_by<"funder"_n, const_mem_fun<funding_attempts_table, uint64_t, &funding_attempts_table::by_funder>>,
    indexed_by<"commodity"_n, const_mem_fun<funding_attempts_table, uint64_t, &funding_attempts_table::by_commodity>>,
    indexed_by<"currency"_n, const_mem_fun<funding_attempts_table, uint64_t, &funding_attempts_table::by_currency>>
> fund_attempts;

