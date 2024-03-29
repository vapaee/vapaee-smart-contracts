// --- markets ---
// List all created markets on the DEX. markets are ALWAYS created in pairs:
// - the lower and even market is the canonical market (Commodity / Currency)
// - the higher and odd market is the inverse market.

// scope: contract
// row: represents a market

TABLE markets_table {
    uint64_t id;
    symbol_code commodity;
    symbol_code currency;
    uint64_t primary_key() const { return id; }
    uint64_t by_commodity_key() const { return commodity.raw(); }
    uint64_t by_currency_key() const { return currency.raw(); }

    uint128_t by_tokens_index() const {
        return vapaee::utils::pack_symbols_in_uint128(commodity, currency);
    }

    string to_string() const {
        return commodity.to_string() + "/" + currency.to_string();
    }
};

typedef eosio::multi_index<"markets"_n, markets_table,
    indexed_by<"commodity"_n, const_mem_fun<markets_table, uint64_t, &markets_table::by_commodity_key>>,
    indexed_by<"currency"_n, const_mem_fun<markets_table, uint64_t, &markets_table::by_currency_key>>,
    indexed_by<"tokensidx"_n, const_mem_fun<markets_table, uint128_t, &markets_table::by_tokens_index>>
> markets;
        
