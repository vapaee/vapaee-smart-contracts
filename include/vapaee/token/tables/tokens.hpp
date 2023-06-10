// --- tokens ---
// Holdes information about all tokens known by this contract, whether they are wrapped tokens or foreign tokens.
// Al accepted tokens must be registered in telosmaindex

// scope: vapaeetokens
// row: indicates the origin of a certain token.

TABLE tokens_table {
    asset  supply;
    name   account;
    uint64_t primary_key() const { return supply.symbol.code().raw(); }
};

typedef eosio::multi_index< "tokens"_n, tokens_table > tokens;
