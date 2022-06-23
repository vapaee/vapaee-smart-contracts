// --- foreign ---
// Holdes information about wrapped tokens as foreign tokens. Al accepted tokens must be registered in telosmaindex

// scope: vapaeetokens
// row: indicates the origin of a certain token.

TABLE foreign_table {
    eosio::symbol_code     symcode;
    name                   account;
    uint64_t primary_key()const { return symcode.raw(); }
};

typedef eosio::multi_index< "foreign"_n, foreign_table > foreign;
