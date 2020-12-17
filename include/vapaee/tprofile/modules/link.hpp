#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/tables.hpp>
#include <vapaee/tprofile/modules/prof.hpp>

using namespace vapaee::tprofile::prof;  // signed_by_any_owner

namespace vapaee {
    namespace tprofile {
        namespace plink {

            void action_add_link(
                string alias,
                string platform,
                string url
            ) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(alias));
                check(profile_iter != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_iter);
                check(owner != "null"_n, "not authorized");

                platforms plat_table(contract, contract.value);
                auto pname_index = plat_table.get_index<"pname"_n>();
                auto plat_iter = pname_index.find(vapaee::utils::hash(platform));
                check(plat_iter != pname_index.end(), "platform not found");

                links link_table(contract, profile_iter->id);
                auto plat_index = link_table.get_index<"platform"_n>();
                auto link_iter = plat_index.find(plat_iter->id);
                check(link_iter == plat_index.end(), "link for this platform already exists");

                pname_index.modify(plat_iter, contract, [&](auto& row) {
                    row.counter++;
                });

                string token = vapaee::utils::prng_token(12);

                link_table.emplace(owner, [&](auto& row) {
                    row.link_id = link_table.available_primary_key();
                    row.platform_id = plat_iter->id;
                    row.url = url;
                    row.token = token;
                    row.witnesses = vector<tuple<uint64_t, uint64_t>>(MAX_WITNESS);
                });

                print(token);
            }

            void action_chg_link(
                string alias,
                uint64_t link_id,
                string url
            ) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(alias));
                check(profile_iter != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_iter);
                check(owner != "null"_n, "not authorized");

                links link_table(contract, profile_iter->id);
                auto link_iter = link_table.find(link_id);
                check(link_iter != link_table.end(), "link not found");

                link_table.modify(link_iter, owner, [&](auto& row) {
                    row.url = url;
                    row.points = 0;
                    row.witnesses.clear();
                });
            }

            void action_witness(
                string witness_alias,
                string link_alias,
                uint64_t link_id
            ) {
                profiles prof_table(contract, contract.value);
                auto alias_index = prof_table.get_index<"alias"_n>();

                auto witness_iter = alias_index.find(vapaee::utils::hash(witness_alias));
                check(witness_iter != alias_index.end(), "profile not found (witness)");

                name owner = signed_by_any_owner(witness_iter);
                check(owner != "null"_n, "not authorized");

                auto link_alias_iter = alias_index.find(vapaee::utils::hash(link_alias));
                check(link_alias_iter != alias_index.end(), "profile not found (link)");

                links link_table(contract, link_alias_iter->id);
                auto link_iter = link_table.find(link_id);
                check(link_iter != link_table.end(), "link not found");

                // there shouldn't be any ram deltas
                link_table.modify(link_iter, contract, [&](auto& row) {
                    tuple<uint64_t, uint64_t> wpair = make_tuple(
                        witness_iter->points,
                        witness_iter->id
                    );

                    // attempt ordered insert
                    auto i = 0;
                    while(i < row.witnesses.size()) {
                        if (get<0>(row.witnesses[i]) < get<0>(wpair))
                            break;
                        i++;
                    }

                    if (i < row.witnesses.size()) {
                        row.witnesses.insert(
                            row.witnesses.begin() + i,
                            wpair
                        );
                        row.points += witness_iter->points;
                    }

                    if (row.witnesses.size() < MAX_WITNESS)
                        row.witnesses.pop_back();
                });
            }

            void action_update_link_points(string alias, uint64_t link_id) {
                profiles prof_table(contract, contract.value);
                auto alias_index = prof_table.get_index<"alias"_n>();

                auto alias_iter = alias_index.find(vapaee::utils::hash(alias));
                check(alias_iter != alias_index.end(), "profile not found");

                links link_table(contract, alias_iter->id);
                auto link_iter = link_table.find(link_id);
                check(link_iter != link_table.end(), "link not found");

                uint64_t link_points = 0;

                for(tuple<uint64_t, uint64_t> witness : link_iter->witnesses) {
                    auto witness_iter = prof_table.find(get<1>(witness));
                    link_points += witness_iter->points;
                }

                // there shouldn't be any ram deltas
                link_table.modify(link_iter, contract, [&](auto& row) {
                    sort(
                        row.witnesses.begin(), 
                        row.witnesses.end(),
                        greater<>()
                    );
                    row.points = link_points;
                });

            }

        };     
    };
};
