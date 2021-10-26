// TABLE nftc
// Represents concrete instances of containers owned by the scope
// scope: owner
TABLE nftc_table {
    uint64_t id;
    uint64_t edition_id;           // reference to the container's edition
    int allitems;                  // amount of all items contained recursively
    int items;                     // amount of items direct (children) whithin this container 
    int freespace;                 // optional variable to reflect freespace (cached value) instead of deep calculating.

    uint64_t primary_key() const { return id; }
    uint64_t by_edition() const { return edition_id; }
    // uint64_t by_version() const { 
    //    return editions.get(edition_id).version_id
    // }
};

typedef eosio::multi_index<"nftcs"_n, nftc_table,
    indexed_by<"edition"_n, const_mem_fun<nftc_table, uint64_t, &nftc_table::by_edition>>
    // , indexed_by<"version"_n, const_mem_fun<nftc_table, uint64_t, &nftc_table::by_version>>
> nftcs;