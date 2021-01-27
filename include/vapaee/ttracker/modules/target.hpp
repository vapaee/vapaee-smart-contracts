#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/ttracker/tables.hpp>
#include <vapaee/tprofile/modules/prof.hpp>

using namespace vapaee::tprofile::prof;  // signed_by_any_owner

namespace vapaee {
    namespace ttracker {
        namespace target {
            
            void action_add_target(
                string alias,
                string season,
                string title,
                string config,
                string map,
                asset lat,
                asset lng
            ) {
                profiles prof_table(tprofile::contract, tprofile::contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_it = alias_index.find(vapaee::utils::hash(alias));
                check(profile_it != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_it);
                check(owner != "null"_n, "not authorized (sig)");

                seasons season_table(contract, contract.value);
                auto season_index = season_table.get_index<"title"_n>();
                auto season_it = season_index.find(vapaee::utils::hash(season));
                check(season_it != season_index.end(), "season not found");

                check(
                    std::find(
                        season_it->tc_whitelist.begin(),
                        season_it->tc_whitelist.end(),
                        profile_it->id
                    ) != season_it->tc_whitelist.end(),
                    "not authorized (wlist)"
                );

                maps map_table(contract, contract.value);
                auto map_index = map_table.get_index<"title"_n>();
                auto map_it = map_index.find(vapaee::utils::hash(map));
                check(map_it != map_index.end(), "map not found");

                targets target_table(contract, season_it->id);
                auto title_index = target_table.get_index<"title"_n>();
                auto trgt_it = title_index.find(vapaee::utils::hash(title));
                check(trgt_it == title_index.end(), "target exists");

                target_table.emplace(owner, [&](auto& row) {
                    row.id = target_table.available_primary_key();
                    row.season = season_it->id;
                    row.title = title;
                    row.created =  time_point_sec(
                        current_time_point().sec_since_epoch()
                    );
                    row.config = config;
                    row.map = map_it->id;
                    row.lat = lat;
                    row.lng = lng;

                    row.whitelist.push_back(profile_it->id);
                });
            }

            void action_add_whitelist(
                string alias,
                string season,
                string target,
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

                targets target_table(contract, season_it->id);
                auto target_index = target_table.get_index<"title"_n>();
                auto trgt_it = target_index.find(vapaee::utils::hash(target));
                check(trgt_it != target_index.end(), "target not found");

                check(trgt_it->whitelist[0] == admin_it->id, "not authorized (wlist)");

                target_index.modify(trgt_it, owner, [&](auto& row) {
                    row.whitelist.push_back(newcreator_it->id);
                });
            }

        };
    };
};
