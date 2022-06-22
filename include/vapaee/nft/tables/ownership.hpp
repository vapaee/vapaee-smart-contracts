// --- ownership ---

// scope: contract
// row: represents an ownership of an NFT for a given concept

TABLE ownership_table {
    uint64_t    nft_id;              // extends an entry on the NFTs table
    name        owner;               // the owner of the NFT

    uint64_t primary_key() const { return nft_id; }
    uint64_t by_owner() const { return owner.value; }
};

typedef eosio::multi_index<"ownerships"_n, ownership_table,
    indexed_by<"owner"_n, const_mem_fun<ownership_table, uint64_t, &ownership_table::by_owner>>
> ownerships;
