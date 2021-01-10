#include <vapaee/base/base.hpp>

// -- membership --
// scope: organization.id
// row: each row is a member of the organization with his roles

TABLE membership {
    uint64_t          profile_id;  // profile id that has this membership
    vector<name>      roles;       // list of roles
    asset             points;      // asbtract field to acumulate some asset
    asset             credits;     // asbtract field to acumulate some asset
    asset             rewards;     // asbtract field to acumulate some asset
    asset             trust;       // asbtract field to acumulate some asset
    asset             rep;         // asbtract field to acumulate some asset

    uint64_t primary_key() const { 
        return profile_id;
    }

    uint64_t by_points() const { 
        return points.amount;
    }

    uint64_t by_credits() const { 
        return credits.amount;
    }

    uint64_t by_rewards() const { 
        return rewards.amount;
    }

    uint64_t by_trust() const { 
        return trust.amount;
    }

    uint64_t by_rep() const { 
        return rep.amount;
    }


};

typedef eosio::multi_index<"members"_n, membership,
    indexed_by<"points"_n, const_mem_fun<membership, uint64_t, &membership::by_points>>,
    indexed_by<"credits"_n, const_mem_fun<membership, uint64_t, &membership::by_credits>>,
    indexed_by<"rewards"_n, const_mem_fun<membership, uint64_t, &membership::by_rewards>>,
    indexed_by<"trust"_n, const_mem_fun<membership, uint64_t, &membership::by_trust>>,
    indexed_by<"rep"_n, const_mem_fun<membership, uint64_t, &membership::by_rep>>
> members;