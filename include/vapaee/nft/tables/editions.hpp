// --- edition ---
// Each concept can have multiple versions and each version can have more than one edition,
// but most will remain in just 1 or 2 at most.
// 
// A new edition can happen when the multimedia or webs references from prevoius edition
// may be broken, or maybe some important info update or maybe just a new look.

// scope: contract
// row: represents an edition of a version of a concept

TABLE edition_table {
    uint64_t id;
    uint64_t version_id;                  // the version which this is a edition of
    vector<tuple<name, uint64_t>> data;   // List of text properties stored in texts table

    uint64_t primary_key() const { return id; }
    uint64_t by_version() const { return version_id; }
};

typedef eosio::multi_index<"editions"_n, edition_table,
    indexed_by<"version"_n, const_mem_fun<edition_table, uint64_t, &edition_table::by_version>>
> editions;