TABLE funding_attempts_table {
    uint64_t market_id;
    asset commodity;
    asset currency;
    
    uint64_t primary_key() const { return market_id; }
    uint64_t by_commodity() const { return commodity.symbol.raw(); }
    uint64_t by_currency() const { return currency.symbol.raw(); }

};

// scope: contract.value
typedef eosio::multi_index<"fundattempts"_n, funding_attempts_table,
    indexed_by<"commodity"_n, const_mem_fun<funding_attempts_table, uint64_t, &funding_attempts_table::by_commodity>>,
    indexed_by<"currency"_n, const_mem_fun<funding_attempts_table, uint64_t, &funding_attempts_table::by_currency>>
> fundattempts;

