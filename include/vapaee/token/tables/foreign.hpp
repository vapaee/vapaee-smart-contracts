// --- foreign ---
// Holdes information about all foreign known by this contract, whether they are wrapped foreign or foreign foreign.
// Al accepted foreign must be registered in telosmaindex

// scope: vapaeeforeign
// row: indicates the origin of a certain token.

TABLE foreign_table {
    symbol_code symcode;
    name        account;
    uint64_t primary_key() const { return symcode.raw(); }
};

typedef eosio::multi_index< "foreign"_n, foreign_table > foreign;
