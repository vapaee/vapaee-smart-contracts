#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/tables.hpp>

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

                name owner = profile_iter->owner;

                require_auth(owner);

                platforms plat_table(contract, contract.value);
                auto pname_index = plat_table.get_index<"pname"_n>();
                auto plat_iter = pname_index.find(vapaee::utils::hash(platform));
                check(plat_iter != pname_index.end(), "platform not found");

                pname_index.modify(plat_iter, contract, [&](auto& row) {
                    row.counter++;
                });

                string token = vapaee::utils::prng_token(12);

                links link_table(contract, owner.value);
                link_table.emplace(owner, [&](auto& row) {
                    row.link_id = link_table.available_primary_key();
                    row.platform_id = plat_iter->id;
                    row.url = url;
                    row.token = token;
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

                name owner = profile_iter->owner;

                require_auth(owner);

                links link_table(contract, owner.value);
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
                check(witness_iter != alias_index.end(), "witness profile not found");

                require_auth(witness_iter->owner);

                auto link_alias_iter = alias_index.find(vapaee::utils::hash(link_alias));
                check(link_alias_iter != alias_index.end(), "link profile not found");

                links link_table(contract, link_alias_iter->owner.value);
                auto link_iter = link_table.find(link_id);
                check(link_iter != link_table.end(), "link not found");

                link_table.modify(link_iter, link_alias_iter->owner, [&](auto& row) {
                    row.points += witness_iter->points;
                    row.witnesses.push_back(witness_iter->id);
                });
            }

        };     
    };
};
