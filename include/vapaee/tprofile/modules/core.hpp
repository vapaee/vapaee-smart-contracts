#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/tables.all.hpp>

namespace vapaee {
    namespace tprofile {
        namespace core {

            void action_add_platform(string platform) {
                require_auth(contract);

                platforms plat_table(contract, contract.value);
                auto pname_index = plat_table.get_index<"pname"_n>();
                auto plat_iter = pname_index.find(vapaee::utils::hash(platform));
                check(plat_iter == pname_index.end(), "identical platform exists");

                plat_table.emplace(contract, [&](auto& row) {
                    row.id = plat_table.available_primary_key();
                    row.pname = platform;
                    row.counter = 0;
                });
            }

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

        };     
    };
};
