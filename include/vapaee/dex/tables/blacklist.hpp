// --- blacklist ---
// This DEX can blacklist tokens recognized as fake, scams, ponzi, etc.

// scope: contract
// row: represent a token that is blacklisted from the DEX DAO system

TABLE blacklist_table {
    uint64_t id;
    symbol_code symbol;
    uint8_t precision;
    name contract;
    name ballot;
    uint64_t primary_key() const { return id; }
    uint64_t by_symbol_key() const { return symbol.raw(); }
};

typedef eosio::multi_index< "blacklist"_n, blacklist_table,
    indexed_by<"symbol"_n, const_mem_fun<blacklist_table, uint64_t, &blacklist_table::by_symbol_key>>
> blacklist;
