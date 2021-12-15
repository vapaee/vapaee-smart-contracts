// --- tokengroups ---
// Each token group is a set of tokens in which the creator can define one of them as currency for the others.
// This way the market of two given commodities can be created, otherwise it is not allowed

// scope: contract
// row: a token group

TABLE tokengroups_table {
    uint64_t id;
    name admin;       // whoever register this group
    string title;
    string website;
    string brief;
    string banner;    // big wide image
    string thumbnail; // small image
    vector<symbol_code> currencies;
    uint64_t primary_key() const { return id; }
};

typedef eosio::multi_index< "tokengroups"_n, tokengroups_table> tokengroups;