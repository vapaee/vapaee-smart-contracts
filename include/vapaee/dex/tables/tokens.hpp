// --- tokens ---
// All tokens MUST be registered in the system. Symbols can NOT be repeated.

// scope: contract
// row: a token

TABLE reg_token_table {
    symbol_code symbol;
    uint8_t precision;
    name contract;
    name admin;
    string title;
    string website;
    string brief;
    string banner;
    string icon;
    string iconlg;
    string pcontact;               // personal contact
    string gcontact;               // group contact
    time_point_sec date;
    uint32_t data;
    uint64_t currency;
    bool tradeable;
    bool stable;
    uint64_t primary_key() const { return symbol.raw(); }
    uint64_t by_contract_key() const { return contract.value; }
    uint64_t by_currency_key() const { return currency; }
};

typedef eosio::multi_index< "tokens"_n, reg_token_table,
    indexed_by<"contract"_n, const_mem_fun<reg_token_table, uint64_t, &reg_token_table::by_contract_key>>,
    indexed_by<"currency"_n, const_mem_fun<reg_token_table, uint64_t, &reg_token_table::by_currency_key>>
> tokens;
