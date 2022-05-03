#pragma once

#include <vapaee/nft/utils.hpp>


namespace vapaee {

    namespace nft {

        class [[eosio::contract]] nftcontainer : public eosio::contract {

            private:
                #include <vapaee/nft/tables.all.hpp>

            public:
                using contract::contract;

                nftcontainer(name receiver, name code, datastream<const char*> ds) :
                    contract(receiver, code, ds)
                    { vapaee::current_contract = receiver; }

                // dapps -----------------------------------------

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
                
                // move a specific NFT to a free slot (nftc_id & position)
                ACTION movernft(
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

                // users -----------------------------------------
                // user is charged for all the RAM used by the Dapp on behalf the user
                ACTION commit(
                    name rampayer
                );

        };  // contract class

    };  // namespace pool

};  // namespace vapaee
