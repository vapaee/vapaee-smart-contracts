#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/profile/tables.all.hpp>

namespace vapaee {
    namespace tprofile {
        namespace core {

            void action_add_profile(name owner, std::string alias) {
                require_auth(owner);

                profiles prof_table(contract, contract.value);

                // check if the exact same account exists
                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(alias));
                check(profile_iter == alias_index.end(), "identical profile exists");

                prof_table.emplace(owner, [&]( auto& row ) {
                    row.id = prof_table.available_primary_key();
                    row.owner = owner;
                    row.alias = alias;
                });
            }

            void action_purge_profile(std::string alias) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(alias));
                check(profile_iter != alias_index.end(), "profile not found");

                require_auth(profile_iter->owner);

                alias_index.erase(profile_iter);
            }

        };     
    };
};
