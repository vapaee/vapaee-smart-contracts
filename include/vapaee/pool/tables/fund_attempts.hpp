#define MARKET_ID_MASK (uint128_t)(((uint128_t)1 << 64) - 1)
#define FOUNDER_MASK   (uint128_t)((((uint128_t)1 << 64) - 1) << 64)


struct [[eosio::table]] funding_attempts_table {
    uint64_t auto_id;
    uint128_t id;
    asset commodity;
    asset currency;

    uint64_t get_pool_id() const {
        return (uint64_t)(id & MARKET_ID_MASK);
    }

    name get_funder() const {
        return name((id & FOUNDER_MASK) >> 64);
    }
    
    uint64_t primary_key() const { return auto_id; }

    uint64_t by_pool_id() const { return get_pool_id(); }
    uint64_t by_funder() const { return get_funder().value; }
    uint64_t by_commodity() const { return commodity.symbol.raw(); }
    uint64_t by_currency() const { return currency.symbol.raw(); }

    uint128_t by_packed_id() const { return id; }
};

// scope: contract.value
typedef eosio::multi_index<"fundattempts"_n, funding_attempts_table,
    indexed_by<"pool"_n, const_mem_fun<funding_attempts_table, uint64_t, &funding_attempts_table::by_pool_id>>,
    indexed_by<"funder"_n, const_mem_fun<funding_attempts_table, uint64_t, &funding_attempts_table::by_funder>>,
    indexed_by<"commodity"_n, const_mem_fun<funding_attempts_table, uint64_t, &funding_attempts_table::by_commodity>>,
    indexed_by<"currency"_n, const_mem_fun<funding_attempts_table, uint64_t, &funding_attempts_table::by_currency>>,
    indexed_by<"idpacked"_n, const_mem_fun<funding_attempts_table, uint128_t, &funding_attempts_table::by_packed_id>>
> fund_attempts;

