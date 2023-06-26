// --- stakers ---
// This is a table for each token containing the stakers of that token.

// scope: symbol code
// row: indicates one staker account of a certain token.

TABLE stakers_table {
    name   account;
    uint64_t primary_key() const { return account.value; }
};

typedef eosio::multi_index< "stakers"_n, stakers_table > stakers;
