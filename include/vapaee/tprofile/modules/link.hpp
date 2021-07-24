#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/base/utils.hpp>

#include <vapaee/tprofile/tables.hpp>
#include <vapaee/tprofile/modules/prof.hpp>


using std::get;
using std::greater;
using std::make_tuple;

using vapaee::tprofile::prof::signed_by_any_owner;


namespace vapaee {
    namespace tprofile {
        namespace plink {

            string aux_create_token_from_url(uint64_t platform_id, string url) {
                string token;
                token = vapaee::utils::prng_token(12);
                return token;
            }

            void action_add_link(
                string alias,
                string platform,
                string url
            ) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_it = alias_index.find(vapaee::utils::hash(alias));
                check(profile_it != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_it);
                check(owner != "null"_n, "not authorized");

                platforms plat_table(contract, contract.value);
                auto pname_index = plat_table.get_index<"pname"_n>();
                auto plat_it = pname_index.find(vapaee::utils::hash(platform));
                check(plat_it != pname_index.end(), "platform not found");

                links link_table(contract, profile_it->id);
                auto plat_index = link_table.get_index<"platform"_n>();
                auto link_it = plat_index.find(plat_it->id);
                check(link_it == plat_index.end(), "link for this platform already exists");

                pname_index.modify(plat_it, contract, [&](auto& row) {
                    row.counter++;
                });

                string token = aux_create_token_from_url(plat_it->id, url);
                

                link_table.emplace(owner, [&](auto& row) {
                    row.id = link_table.available_primary_key();
                    row.platform_id = plat_it->id;
                    row.url = url;
                    row.token = token;
                    row.proof = string();
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
                auto profile_it = alias_index.find(vapaee::utils::hash(alias));
                check(profile_it != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_it);
                check(owner != "null"_n, "not authorized");

                links link_table(contract, profile_it->id);
                auto link_it = link_table.find(link_id);
                check(link_it != link_table.end(), "link not found");

                link_table.modify(link_it, owner, [&](auto& row) {
                    string token = aux_create_token_from_url(row.platform_id, url);
                    row.url = url;
                    row.points = 0;
                    row.token = token;
                    row.proof = string();
                    row.witnesses.clear();
                });
            }

            void action_set_link_proof(
                string alias,
                uint64_t link_id,
                string proof_url
            ) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_it = alias_index.find(vapaee::utils::hash(alias));
                check(profile_it != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_it);
                check(owner != "null"_n, "not authorized");

                links link_table(contract, profile_it->id);
                auto link_it = link_table.find(link_id);
                check(link_it != link_table.end(), "link not found");

                link_table.modify(link_it, owner, [&](auto& row) {
                    row.points = 0;
                    row.proof = proof_url;
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

                auto witness_it = alias_index.find(vapaee::utils::hash(witness_alias));
                check(witness_it != alias_index.end(), "profile not found (witness)");

                name owner = signed_by_any_owner(witness_it);
                check(owner != "null"_n, "not authorized");

                auto link_alias_it = alias_index.find(vapaee::utils::hash(link_alias));
                check(link_alias_it != alias_index.end(), "profile not found (link)");

                links link_table(contract, link_alias_it->id);
                auto link_it = link_table.find(link_id);
                check(link_it != link_table.end(), "link not found");

                // there shouldn't be any ram deltas
                link_table.modify(link_it, contract, [&](auto& row) {
                    tuple<uint64_t, uint64_t> wpair = make_tuple(
                        witness_it->points,
                        witness_it->id
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
                        row.points += witness_it->points;
                    }

                    if (row.witnesses.size() < MAX_WITNESS) {
                        row.points -= get<0>(row.witnesses.back());
                        row.witnesses.pop_back();
                    }
                        
                });
            }

            void action_update_link_points(string alias, uint64_t link_id) {
                profiles prof_table(contract, contract.value);
                auto alias_index = prof_table.get_index<"alias"_n>();

                auto alias_it = alias_index.find(vapaee::utils::hash(alias));
                check(alias_it != alias_index.end(), "profile not found");

                links link_table(contract, alias_it->id);
                auto link_it = link_table.find(link_id);
                check(link_it != link_table.end(), "link not found");

                uint64_t link_points = 0;

                for(tuple<uint64_t, uint64_t> witness : link_it->witnesses) {
                    auto witness_it = prof_table.find(get<1>(witness));
                    link_points += witness_it->points;
                }

                // there shouldn't be any ram deltas
                link_table.modify(link_it, contract, [&](auto& row) {
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
