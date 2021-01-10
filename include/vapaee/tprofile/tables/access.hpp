#include <vapaee/base/base.hpp>

// -- access --
// scope: profile.id
// row: represent a delegation of trust from profile to granted to execute a specific action on a specific smart contract.

TABLE grant {
    uint64_t           id; // auto-increment
    name         contract; // account name of the smart contract      
    name           action; // name of the action
    string   organization;
    name          granted; // org's smart contract which is granted with access to execute contract.action on behalf of this profile

    uint64_t primary_key() const {
        return id;
    }
};

typedef eosio::multi_index<"access"_n, grant> access;
