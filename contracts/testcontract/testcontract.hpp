#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/tables.all.hpp>

using namespace eosio;
using namespace std;

namespace testing {

    static name contract = eosio::name("testcontract");

    CONTRACT testcontract : public eosio::contract {
        public:
            using contract::contract;

            ACTION gaccesstest(name caller, string alias) {
                profiles prof_table(tprofile::contract, tprofile::contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(alias));
                check(profile_iter != alias_index.end(), "profile not found");

                access grant_table(tprofile::contract, profile_iter->id);
                auto grant_index = grant_table.get_index<"accesscheck"_n>();

                const array<uint128_t, 2> sig = {
                    (uint128_t)caller.value,
                    ((uint128_t)contract.value << 64) | ("gaccesstest"_n).value
                };

                auto grant_iter = grant_index.find(fixed_bytes<32>(sig));
                check(grant_iter != grant_index.end(), "access denied");
                print("access granted");
            }
    };

};
