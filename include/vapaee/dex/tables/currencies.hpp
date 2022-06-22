// --- currencies ---
// All markets (pools and order books) must have at least one currency in the token pair, but not all tokens are currencies.
// Only tokens declared in this table are considered currencies.

// scope: contract
// row: a currency information

TABLE currencies_table {
    uint64_t id;
    string website;
    string brief;
    symbol_code currency;
    uint64_t primary_key() const { return id; }
};

typedef eosio::multi_index< "currencies"_n, currencies_table> currencies;