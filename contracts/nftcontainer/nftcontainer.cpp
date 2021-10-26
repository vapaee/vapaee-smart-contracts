#include "nftcontainer.hpp"

#include <vapaee/base/utils.hpp>

using std::strtoull;
using std::to_string;

using vapaee::nft::nftcontainer;


// register a new DApp in dapps
void nftcontainer::regdapp(
    uint64_t profile_id, 
    name contract
) {
    // signed by owner of profile
}

// register a new data for an enrty (concept or edition)
void nftcontainer::newdata(
    name     table,
    uint64_t scope,
    uint64_t key,
    name     label,
    string   data
) {
    // signed by dapp
    // table, scope & key identifies a concept or an edition (of a version of a concept).
    // That concept has dapp_id property 
}

// update existing data
void nftcontainer::upddata(
    uint64_t text_id,
    string   data
) {
    // signed by dapp
    // table, scope & key identifies a concept or an edition (of a version of a concept).
    // That concept has dapp_id property     
}

// register a new concept for that DApp
void nftcontainer::regconcept(
    uint64_t dapp_id,
    name     title,
    bool     is_container,
    bool     is_nft
) {
    // signed by dapp
}

// register a new version for a given concept
void nftcontainer::newversion(
    string title,
    uint64_t concept_id,
    uint64_t author
) {
    // signed by dapp (can be taken from the concept)
}

// register a new edition for a given version
void nftcontainer::newedition(
    string version_title,
    uint64_t author
) {
    // signed by dapp (can be taken from the concept)
}

// issue several instances of a concept specific version and edition
void nftcontainer::issuenft(
    name owner, 
    uint64_t edition_id, 
    uint64_t quantity
) {
    // signed by dapp
}

// Creates a permanent container for the user.
void nftcontainer::newcontainer(
    name owner, 
    uint64_t edition_id
) {
    // signed by dapp
}

// take the existing NFT (which has quantity > 1) and create a second NFT with a separated amount
void nftcontainer::splitnft(
    name owner, 
    uint64_t nft_id, 
    uint64_t quantity
) {
    // signed by dapp
}

// move a specific NFT to a free slot (nftc_id & position)
void nftcontainer::movernft(
    name owner, 
    uint64_t nft_id, 
    uint64_t nftc_id, 
    uint64_t position
) {
    // signed by dapp
}

// specific NFT switch positions with the NFT contained in the given slot (nftc_id & position)
void nftcontainer::switchnfts(
    name owner, 
    uint64_t nft_id, 
    uint64_t nftc_id, 
    uint64_t position
) {
    // signed by dapp
}

// transfer an NFT to another account with optional memo
void nftcontainer::transfer(
    name from,
    name to, 
    uint64_t nft_id, 
    string memo
) {
    // signed by from
}

// users -----------------------------------------
// user is charged for all the RAM used by the Dapp on behalf the user
void nftcontainer::commit(name rampayer) {
    // signed by rampayer
}               
            
