#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/ttracker/tables.hpp>
#include <vapaee/tprofile/modules/prof.hpp>

using vapaee::tprofile::prof::signed_by_any_owner;

namespace vapaee {
    namespace ttracker {
        namespace map {

            void action_add_map(
                string alias,
                string title,
                string config
            ) {
                profiles prof_table(tprofile::contract, tprofile::contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_it = alias_index.find(vapaee::utils::hash(alias));
                check(profile_it != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_it);
                check(owner != "null"_n, "not authorized");

                maps map_table(contract, contract.value);
                auto title_index = map_table.get_index<"title"_n>();
                auto map_it = title_index.find(vapaee::utils::hash(title));
                check(map_it == title_index.end(), "map exists");

                map_table.emplace(owner, [&](auto& row) {
                    row.id = map_table.available_primary_key();
                    row.title = title;
                    row.config = config;
                    row.creator = profile_it->id;
                });
            }

        };

    };

};
