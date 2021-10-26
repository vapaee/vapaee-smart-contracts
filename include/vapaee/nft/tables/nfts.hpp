// TABLE nft
// scope: owner
            
struct slot_struct {
    uint64_t        nftc_id;       // Referencia a la instancia de contenedor que contiene al NFT.
    uint64_t        position;      // Indica en que posición del contenedor se encuentra el NFT.
    time_point_sec  timestamp;     // Cuándo fue la última actualización (cuanto hace que el NFT está aquí?)
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