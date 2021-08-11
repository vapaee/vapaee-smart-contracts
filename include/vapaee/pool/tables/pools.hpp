struct [[eosio::table]] pool_table {
    uint64_t id;
    asset commodity_reserve;
    asset currency_reserve;

    uint64_t primary_key() const { return id; }
    uint128_t by_symbols() const {
        return symbols_get_index(
            commodity_reserve.symbol.code(),
            currency_reserve.symbol.code()
        );
    }
    uint64_t by_commodity_key() const { return commodity_reserve.symbol.raw(); }
    uint64_t by_currency_key() const { return currency_reserve.symbol.raw(); }
};

// scope: contract.value
typedef eosio::multi_index<"pools"_n, pool_table,
    indexed_by<"symbols"_n, const_mem_fun<pool_table, uint128_t, &pool_table::by_symbols>>,
    indexed_by<"commodity"_n, const_mem_fun<pool_table, uint64_t, &pool_table::by_commodity_key>>,
    indexed_by<"currency"_n, const_mem_fun<pool_table, uint64_t, &pool_table::by_currency_key>>
> pools;

