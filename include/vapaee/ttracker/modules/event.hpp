#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/ttracker/tables.hpp>
#include <vapaee/tprofile/modules/prof.hpp>

using namespace vapaee::tprofile::prof;  // signed_by_any_owner

namespace vapaee {
    namespace ttracker {
        namespace event {
            
            void action_add_event(
                string alias,
                string season,
                string target,
                string map,
                asset lat,
                asset lng,
                string geometry,
                string config
            ) {
                profiles prof_table(tprofile::contract, tprofile::contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(alias));
                check(profile_iter != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_iter);
                check(owner != "null"_n, "not authorized (sig)");

                seasons season_table(contract, contract.value);
                auto season_index = season_table.get_index<"title"_n>();
                auto season_iter = season_index.find(vapaee::utils::hash(season));
                check(season_iter != season_index.end(), "season not found");

                maps map_table(contract, contract.value);
                auto map_index = map_table.get_index<"title"_n>();
                auto map_iter = map_index.find(vapaee::utils::hash(map));
                check(map_iter != map_index.end(), "map not found");

                targets target_table(contract, season_iter->id);
                auto target_index = target_table.get_index<"title"_n>();
                auto target_iter = target_index.find(vapaee::utils::hash(target));
                check(target_iter != target_index.end(), "target not found");

                check(
                    std::find(
                        target_iter->whitelist.begin(),
                        target_iter->whitelist.end(),
                        profile_iter->id
                    ) != target_iter->whitelist.end(),
                    "not authorized (wlist)"
                );

                events event_table(contract, season_iter->id);
                event_table.emplace(owner, [&](auto& row) {
                    row.id = event_table.available_primary_key();
                    row.target = target_iter->id;
                    row.profile = profile_iter->id;
                    row.date = time_point_sec(
                        current_time_point().sec_since_epoch()
                    );
                    row.map = map_iter->id;
                    row.lat = lat;
                    row.lng = lng;
                    row.geometry = geometry;
                    row.config = config;
                });

            }
           
        };
    };
};
