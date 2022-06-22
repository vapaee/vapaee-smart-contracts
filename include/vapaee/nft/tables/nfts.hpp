// -- nft --
// Each NFT is a concept that can be issued several times, having each copy its own id.
// Each concept can have multiple versions (even from diferent authors) and each version can have multiple editions (by the same author).
// And finally, each edition can have multiple copies (or for a better name, NFTs).
// This table refers to the NFTs that for a given concept.

// The slot property is optional and should be used to store the position of the NFT in the container (if it make any sence). Reed more about slots in the containers table file.

// scope: contract
// row: represents an NFT instance (or copy) and where is located (slot)

struct slot_struct {
    uint64_t        container_nft_id; // Reference to "the other" instance of NFT in the relationship "container-content"
    uint64_t        transformation;   // Indicates where the NFT is inside the container.
    time_point_sec  timestamp;        // When was the last update (how long has the NFT been here?)
};

TABLE nft_table {
    uint64_t    id;
    uint64_t    edition_id;        // Reference the edition that was in effect when the nft was created.
    slot_struct slot;              // indicates where the NFT is

    uint64_t primary_key() const { return id; }
    uint64_t by_edition() const {
        return edition_id;
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

typedef eosio::multi_index<"nfts"_n, nft_table,
    indexed_by<"edition"_n, const_mem_fun<nft_table, uint64_t, &nft_table::by_edition>>,
    indexed_by<"version"_n, const_mem_fun<nft_table, uint64_t, &nft_table::by_version>>,
    indexed_by<"concept"_n, const_mem_fun<nft_table, uint64_t, &nft_table::by_concept>>
> nfts;