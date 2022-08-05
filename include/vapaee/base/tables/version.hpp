// --- version ---
// Each vapaee contract has a version table on the RAM with a unique entry holding the current version of the contract.

// scope: contract
// row: a unique row holding the version of the contract

TABLE version_table {
    string version;
    uint64_t primary_key() const { return 1; }
};

typedef eosio::multi_index<"version"_n, version_table > version;