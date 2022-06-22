// -- container --
// Any NFT container is itself an NFT. But using this table it also can be a container for other NFTs.
// This table expands the nft table for representing those NFTs that are also a container.

// The container.slots property is an optional list of slots in which the container can hold NFTs. This information is used redundantly stored in the nft.slot property. It can be used in any direction, in both or none depending on the devs needs. Defaults is bidirectional, which means, every entry this list hast (with the nft_id pointing to con item contained) will have a corresponding entry in the nft.slot item-side of the relationship "container-content".

// scope: contract
// row: represents the "container side" of an NFT

TABLE container_table {
    uint64_t nft_id;               // extends this NFTs from nfts table
    int allitems;                  // amount of all items contained recursively (for NFT-trees)
    int items;                     // amount of items direct (children) whithin this container 
    int freespace;                 // optional variable to reflect freespace (cached value) instead of deep calculating.
    bool valid;                    // the container may have some specific requirements to be valid
    vector<slot_struct> slots;     // indicates where the NFTs are

    uint64_t primary_key() const { return nft_id; }
    
};

typedef eosio::multi_index<"containers"_n, container_table> containers;