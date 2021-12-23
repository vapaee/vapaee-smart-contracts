// --- text ---
// Some instances will have associated a small amount of data that defines
// behaviors, aesthetics, and references external content. This text is structured
// in a free text (plain, json, html, xml, etc) and attached to the instance using this table.

// scope: contract
// row: represents a text extension for any given entry

TABLE text_table {
    uint64_t id;
    name     table;                // table holding the entry which is being extended with this data
    name     scope;                // scope used to instantiate the table for the entry
    uint64_t key;                  // key for the entry extended
    name    label;                 // Label that identifies it within the entry namespace
    string   text;                 // free text containing dapp specific information related to

    uint64_t primary_key() const { return id; }
};

typedef eosio::multi_index<"texts"_n, text_table> texts;