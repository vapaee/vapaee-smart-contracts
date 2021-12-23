// --- dapp ---
// Each profile can register as a dapp, adding the contract name.

// scope: contract
// row: represents a dapp 

TABLE dapp_table {
    uint64_t profile_id;           // reference to a profile (from telosprofile contract), who would be the profile associated with the app.
    name     contract;             // smart contract that interacts with nftconteiner using dapp privileges

    uint64_t primary_key() const { return profile_id; }
    uint64_t by_contract() const { return contract.value; }
};

typedef eosio::multi_index<"dapps"_n, dapp_table,
    indexed_by<"contract"_n, const_mem_fun<dapp_table, uint64_t, &dapp_table::by_contract>>
> dapps;