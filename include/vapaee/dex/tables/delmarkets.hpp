// --- delmarkets ---
// When a token is banned all markets that use it are deleted.
// This is a slow process, during which the markets are not available.

// scope: contract
// row: indicates the market with the given id is deleted

TABLE delmarkets_table {
    uint64_t id;            // market id
    uint64_t primary_key() const { return id; }
};

typedef eosio::multi_index<"delmarkets"_n, delmarkets_table> delmarkets;
