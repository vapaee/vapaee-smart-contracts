// --- deps ---
// Then creating a Telos Decide ballot, users need to pay a fee. THis table list the accounts who has paid for that fee.

// scope: contract
// row: account who has paid the fee for a dao ballot

TABLE deps_table {
    name feepayer;
    uint64_t primary_key() const { return feepayer.value; }
};

typedef eosio::multi_index< "deps"_n, deps_table > deps;
