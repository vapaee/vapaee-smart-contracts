// --- tokendata ---
// Al tokens will have a description page with the basic fields, but this can be extended.
// This table is a simple way to store extra data of a token to show in the token's page.
// It can be a bicture, a video, a tweet, a twitter account, etc.

// scope: symbol_code
// row: represents an entra entry on the page of a token

TABLE tokendata_table {
    uint64_t id;
    string text;
    string link;
    name shownas;
    time_point_sec date;
    uint64_t primary_key() const { return id; }
    uint64_t by_shownas_key() const { return shownas.value; }
};

typedef eosio::multi_index< "tokendata"_n, tokendata_table,
    indexed_by<"shownas"_n, const_mem_fun<tokendata_table, uint64_t, &tokendata_table::by_shownas_key>>
> tokendata;