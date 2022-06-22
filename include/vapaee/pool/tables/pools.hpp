// --- pools ---
// Each canonical market has a default converter, therefore, the id in both tables is the same.
// This converter is the one that is used to manage the liquidity of the market pool.

// scope: contract
// row: a default converter/pool for a market

TABLE pool_table {
    uint64_t market_id;

    asset commodity_reserve;
    asset currency_reserve;

    asset total_participation;
    asset fee;

    uint64_t primary_key() const { return market_id; }
    uint128_t by_symbols() const {
        return vapaee::utils::symbols_get_index(
            commodity_reserve.symbol.code(),
            currency_reserve.symbol.code()
        );
    }
    uint64_t by_commodity_key() const { return commodity_reserve.symbol.code().raw(); }
    uint64_t by_currency_key() const { return currency_reserve.symbol.code().raw(); }

    string to_string() const {
        return commodity_reserve.symbol.code().to_string() + "/" + currency_reserve.symbol.code().to_string();
    }
};

typedef eosio::multi_index<"pools"_n, pool_table,
    indexed_by<"symbols"_n, const_mem_fun<pool_table, uint128_t, &pool_table::by_symbols>>,
    indexed_by<"commodity"_n, const_mem_fun<pool_table, uint64_t, &pool_table::by_commodity_key>>,
    indexed_by<"currency"_n, const_mem_fun<pool_table, uint64_t, &pool_table::by_currency_key>>
> pools;
