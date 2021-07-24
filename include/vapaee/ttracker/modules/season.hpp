#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/ttracker/tables.hpp>
#include <vapaee/tprofile/modules/prof.hpp>

using vapaee::tprofile::prof::signed_by_any_owner;

namespace vapaee {
    namespace ttracker {
        namespace season { 

            void action_add_season( 
                string alias,
                string title,
                time_point_sec start,
                time_point_sec finish,
                string config
            ) {
                profiles prof_table(tprofile::contract, tprofile::contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_it = alias_index.find(vapaee::utils::hash(alias));
                check(profile_it != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_it);
                check(owner != "null"_n, "not authorized");

                seasons season_table(contract, contract.value);
                auto title_index = season_table.get_index<"title"_n>();
                auto season_it = title_index.find(vapaee::utils::hash(title));
                check(season_it == title_index.end(), "season exists");

                season_table.emplace(owner, [&](auto& row) {
                    row.id = season_table.available_primary_key();
                    row.title = title;
                    row.start = start;
                    row.finish = finish;
                    row.config = config;

                    row.tc_whitelist.push_back(profile_it->id);
                });
            }

            void action_add_target_creator_whitelist(
                string alias,
                string season,
                string new_alias
            ) {
                profiles prof_table(tprofile::contract, tprofile::contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto admin_it = alias_index.find(vapaee::utils::hash(alias));
                check(admin_it != alias_index.end(), "profile not found (admin)");

                name owner = signed_by_any_owner(admin_it);
                check(owner != "null"_n, "not authorized (sig)");

                auto newcreator_it = alias_index.find(vapaee::utils::hash(new_alias));
                check(newcreator_it != alias_index.end(), "profile not found (new)");

                seasons season_table(contract, contract.value);
                auto season_index = season_table.get_index<"title"_n>();
                auto season_it = season_index.find(vapaee::utils::hash(season));
                check(season_it != season_index.end(), "season not found");

                check(season_it->tc_whitelist[0] == admin_it->id, "not authorized (wlist)");

                season_index.modify(season_it, owner, [&](auto& row) {
                    row.tc_whitelist.push_back(newcreator_it->id);
                });
            }
        };
    };
};
