// --- version ---
// Each concept can have multiple versions made by other profiles (not only dapps).
// Each version for the same concept will share properties among all its final NFTs.

// scope: contract
// row: represents a version of a concept

TABLE version_table {
    uint64_t id;
    string title;                  // Text identifying the version
    name label;                    // Repeats concept label (for reading purposes)
    uint64_t concept_id;           // the concept which this is a version of
    uint64_t author;               // author is a profile from telosprofile
    uint64_t current_edition;      // reference to the latest edition

    uint64_t primary_key() const { return id; }
    checksum256 by_title() const { return vapaee::utils::hash(title); }
    uint64_t by_concept() const { return concept_id; }
};

typedef eosio::multi_index<"versions"_n, version_table,
    indexed_by<"title"_n, const_mem_fun<version_table, checksum256, &version_table::by_title>>,
    indexed_by<"concept"_n, const_mem_fun<version_table, uint64_t, &version_table::by_concept>>
> versions;
