#include <vapaee/base/base.hpp>

// -- membership --
// scope: organization.id
// row: each row is a member of the organization with his roles

struct [[eosio::table]] membership_t {
    uint64_t     profile_id; // profile id that has this membership
    vector<name> roles;      // list of roles
    
    // asbtract fields to acumulate some asset
    asset index1;
    asset index2;
    asset index3;
    asset index4;
    asset index5;

    uint64_t primary_key() const { 
        return profile_id;
    }

    uint64_t by_index1() const { 
        return index1.amount;
    }

    uint64_t by_index2() const { 
        return index2.amount;
    }

    uint64_t by_index3() const { 
        return index3.amount;
    }

    uint64_t by_index4() const { 
        return index4.amount;
    }

    uint64_t by_index5() const { 
        return index5.amount;
    }


};

typedef eosio::multi_index<"members"_n, membership_t,
    indexed_by<"index1"_n, const_mem_fun<membership_t, uint64_t, &membership_t::by_index1>>,
    indexed_by<"index2"_n, const_mem_fun<membership_t, uint64_t, &membership_t::by_index2>>,
    indexed_by<"index3"_n, const_mem_fun<membership_t, uint64_t, &membership_t::by_index3>>,
    indexed_by<"index4"_n, const_mem_fun<membership_t, uint64_t, &membership_t::by_index4>>,
    indexed_by<"index5"_n, const_mem_fun<membership_t, uint64_t, &membership_t::by_index5>>
> members;
