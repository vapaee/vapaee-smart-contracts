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

            void action_update_points(string alias) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(alias));
                check(profile_iter != alias_index.end(), "profile not found");

                /*
                    points calculation

                    platform_weighted_sum = [
                        total_platform_profile_score * platform_global_count
                        for platform in platforms
                    ]

                    points = 1 + floor(
                        (platform_weighted_sum) / total_platforms_global_count
                    )
                */

                // save user points in each platform to an array indexed by platform id
                platforms plat_table(contract, contract.value);
                auto plat_end = plat_table.end();

                uint64_t platform_count = plat_end->id + 1;
                uint64_t profile_totals[platform_count];

                links link_table(contract, profile_iter->owner.value);
                auto link_iter = link_table.begin();
                while(link_iter != link_table.end()) {
                    profile_totals[link_iter->platform_id] = link_iter->points;
                    link_iter++;
                }

                // iterate over platforms and perform weighted sum, also save total link amount
                uint64_t platforms_total_count = 0;
                uint64_t weighted_sum = 0;

                auto plat_iter = plat_table.begin();
                while(plat_iter != plat_end) {
                    weighted_sum += profile_totals[plat_iter->id] * plat_iter->counter;
                    platforms_total_count += plat_iter->counter;
                    plat_iter++;
                }

                // finally update points
                alias_index.modify(profile_iter, profile_iter->owner, [&](auto& row) {
                    row.points = 1 + floor((float)(weighted_sum) / (float)(platforms_total_count));
                    row.kyclevel = floor(log2(row.points));
                });

            }

        };     
    };
};