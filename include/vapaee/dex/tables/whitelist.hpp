// --- whitelist ---
// The same way this DAO DX allows to blacklist a token blocking it inmediately from operating,
// it allows to whitelist a token to recognize it in the community and to prevent it from being blocked.

// scope: contract
// row: a saved token

TABLE whitelist_table {
    symbol_code symbol;
    name contract;
    name ballot;
    uint64_t primary_key() const { return symbol.raw(); }
};

typedef eosio::multi_index< "whitelist"_n, whitelist_table> whitelist;
