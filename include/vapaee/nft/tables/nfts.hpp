// --- nft ---
// Each NFT is a concept that can be instantiated as a container and/or as an NFT itself.
// Given that each concept can have multiple versions and each version can have multiple editions,
// each NFT points to the edition of a version of a concept to which it belongs.

// scope: owner
// row: represents an NFT instance and where is located (slot)

struct slot_struct {
    uint64_t        nftc_id;       // Reference to the container instance that contains the NFT.
    uint64_t        position;      // Indicates where the NFT is inside the container.
    time_point_sec  timestamp;     // When was the last update (how long has the NFT been here?)
};

TABLE nft_table {
    uint64_t    id;
    uint64_t    edition_id;        // Reference the edition that was in effect when the nft was created.
    slot_struct slot;              // indicates where the NFT is

    uint64_t primary_key() const { return id; }
    uint64_t by_edition() const { return edition_id; }
    // uint64_t by_version() const { 
    //    return editions.get(edition_id).version_id
    // }
};

typedef eosio::multi_index<"nfts"_n, nft_table,
    indexed_by<"edition"_n, const_mem_fun<nft_table, uint64_t, &nft_table::by_edition>>
    // , indexed_by<"version"_n, const_mem_fun<nft_table, uint64_t, &nft_table::by_version>>
> nfts;