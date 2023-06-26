// --- holders ---
// This is a table for each token containing the holders of that token.

// scope: symbol code
// row: indicates the holder of a certain token.

TABLE holders_table {
    name   account;
    uint64_t primary_key() const { return account.value; }
};

typedef eosio::multi_index< "holders"_n, holders_table > holders;
