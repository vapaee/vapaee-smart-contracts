// --- userorders ---
// This table is a summary of the user orders for a given market.

// scope: owner
// row: a user order summary for a given market

TABLE userorders_table {
    uint64_t market;
    string table;
    vector<uint64_t> ids;
    uint64_t primary_key() const { return market; }
};

typedef eosio::multi_index<"userorders"_n, userorders_table > userorders;