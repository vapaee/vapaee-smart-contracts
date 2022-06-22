// --- deposits ---
// Book dex needs to have access to the users tokens. So the user needs to deposits funds before ordering.

// scope: owner
// row: owner deposit for a given asset

TABLE deposits_table {
    asset amount;
    uint64_t primary_key() const { return amount.symbol.code().raw(); }
};

typedef eosio::multi_index< "deposits"_n, deposits_table > deposits;
