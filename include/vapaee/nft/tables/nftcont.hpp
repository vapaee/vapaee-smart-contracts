// TABLE nftcont
// If an NFT is itself a container, then here is where we
// indicate where to find the asociated instantiated container.
// scope: owner
TABLE nftcont_table {
    uint64_t nft_id;          // extends this NFTs from nfts table
    uint64_t nftc_id;         // reference to the instance of the container asociated with this NFT

    uint64_t primary_key() const { return nft_id; }
    uint64_t by_nftc() const { return nftc_id; }
};

typedef eosio::multi_index<"nftcont"_n, nftcont_table,
    indexed_by<"nftc"_n, const_mem_fun<nftcont_table, uint64_t, &nftcont_table::by_nftc>>
> nftcont;