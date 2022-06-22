#pragma once

#include <vapaee/nft/utils.hpp>


namespace vapaee {

    CONTRACT nftcontainer : public eosio::contract {

        private:
            #include <vapaee/nft/tables.all.hpp>

        public:
            using contract::contract;

            nftcontainer(name receiver, name code, datastream<const char*> ds) :
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver; }

                // ------- Dapp module ------
                // register a new DApp in dapps
                ACTION regdapp(
                    uint64_t profile_id, 
                    name contract
                );

                // register a new concept for that DApp
                ACTION regconcept(
                    uint64_t dapp_id,
                    name     title,
                    bool     is_container,
                    bool     is_nft
                );

                // ------- Data module ------
                // register a new data for an enrty (concept or edition)
                ACTION newdata(
                    name     table,
                    uint64_t scope,
                    uint64_t key,
                    name     label,
                    string   data
                );

                // update existing data
                ACTION upddata(
                    uint64_t text_id,
                    string   data
                );


                // register a new data for an enrty (concept or edition)
                ACTION newdump(
                    uint64_t concept,
                    uint64_t id,
                    string   memdump
                );


                // update existing data
                ACTION upddump(
                    uint64_t dump_id,
                    string   memdump
                );

                // ------- Creators module ------
                // register a new version for a given concept
                ACTION newversion(
                    string title,
                    uint64_t concept_id,
                    uint64_t author
                );

                // register a new edition for a given version
                ACTION newedition(
                    string version_title,
                    uint64_t author
                );

                // issue several instances of a concept specific version and edition
                ACTION issuenft(
                    name owner, 
                    uint64_t edition_id, 
                    uint64_t quantity
                );

                // Creates a permanent container for the user.
                ACTION newcontainer(
                    name owner, 
                    uint64_t edition_id
                );

                // take the existing NFT (which has quantity > 1) and create a second NFT with a separated amount
                ACTION splitnft(
                    name owner, 
                    uint64_t nft_id, 
                    uint64_t quantity
                );

                // users -----------------------------------------
                // user is charged for all the RAM used by the Dapp on behalf the user
                ACTION commit(name rampayer);

                // --- Owners Module ---

                // move a specific NFT to a free slot (nftc_id & position)
                ACTION movenft(
                    name owner, 
                    uint64_t nft_id, 
                    uint64_t nftc_id, 
                    uint64_t position
                );

                // specific NFT switch positions with the NFT contained in the given slot (nftc_id & position)
                ACTION switchnfts(
                    name owner, 
                    uint64_t nft_id, 
                    uint64_t nftc_id, 
                    uint64_t position
                );

                // transfer an NFT to another account with optional memo
                ACTION transfer(
                    name from,
                    name to,
                    uint64_t nft_id, 
                    string memo
                );

                // transfer many NFTs represented uin the same entry with a quantity property grather or equal to quantity param
                ACTION transfermany(
                    name from,
                    name to, 
                    uint64_t nft_id,
                    uint64_t quantity,
                    string memo
                );

                // --- Marbel Compatibility Module ---

                //mint a new item
                //auth: manager
                ACTION mintitem(name to, name group_name);

                //transfer ownership of one or more items
                //auth: owner
                ACTION transferitem(name from, name to, vector<uint64_t> serials, string memo);

                //activate an item
                //auth: owner
                ACTION activateitem(uint64_t serial);

                //reclaim an item from the owner
                //auth: manager
                ACTION reclaimitem(uint64_t serial);

                //consume an item
                //post: inline releaseall() if bond(s) exist
                //auth: owner
                ACTION consumeitem(uint64_t serial);

                //destroy an item
                //post: inline releaseall() if bond(s) exist
                //auth: manager
                ACTION destroyitem(uint64_t serial, string memo);




    };  // contract class

};  // namespace vapaee
