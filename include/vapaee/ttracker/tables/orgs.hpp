#include <vapaee/base/base.hpp>

// -- orgs --
// scope: contract
// row: each row is a whitelisted organization

TABLE org {
    uint64_t           id;
    string       org_name;
    name     org_contract;

    uint64_t primary_key() const {
        return id;
    }
};

typedef eosio::multi_index<"orgs"_n, org> orgs;
