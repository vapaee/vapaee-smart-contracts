// -- owner's --
// Each NFT is represented in the nft table. This table uses the user as scope to list all the NFTs owned by that user.

// scope: owner
// row: represents an NFT instance (or copy) owned by the owner

TABLE owners_table {
    uint64_t    nft_id;            // extends an entry on the NFTs table

    uint64_t primary_key() const { return nft_id; }

    uint64_t by_edition() const {
        nfts tbl(vapaee::nft::contract, vapaee::nft::contract.value);
        return tbl.get(nft_id).edition_id;
    }
    uint64_t by_version() const { 
        editions tbl(vapaee::nft::contract, vapaee::nft::contract.value);
        return tbl.get(by_edition()).version_id;
    }
    uint64_t by_concept() const {
        versions tbl(vapaee::nft::contract, vapaee::nft::contract.value);
        return tbl.get(by_version()).concept_id;
    }
    
};

typedef eosio::multi_index<"owners"_n, owners_table,
    indexed_by<"edition"_n, const_mem_fun<owners_table, uint64_t, &owners_table::by_edition>>,
    indexed_by<"version"_n, const_mem_fun<owners_table, uint64_t, &owners_table::by_version>>,
    indexed_by<"concept"_n, const_mem_fun<owners_table, uint64_t, &owners_table::by_concept>>
> owners;