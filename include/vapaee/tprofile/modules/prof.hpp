#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/tables.hpp>

namespace vapaee {
    namespace tprofile {
        namespace prof {

            void action_add_profile(name owner, string alias) {
                require_auth(owner);

                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(alias));
                check(profile_iter == alias_index.end(), "identical profile exists");

                prof_table.emplace(owner, [&](auto& row) {
                    row.id = prof_table.available_primary_key();
                    row.owner = owner;
                    row.alias = alias;
                });
            }

            void action_chg_profile(string old_alias, string new_alias) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(old_alias));
                check(profile_iter != alias_index.end(), "profile not found");

                name owner = profile_iter->owner;

                require_auth(owner);

                auto newprofile_iter = alias_index.find(vapaee::utils::hash(new_alias));
                check(newprofile_iter == alias_index.end(), "identical profile exists");

                alias_index.modify(profile_iter, owner, [&](auto& row) {
                    row.alias = new_alias;
                });
            }

        };     
    };
};