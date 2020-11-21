#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/tables.all.hpp>

namespace vapaee {
    namespace tprofile {
        namespace core {

            void action_add_platform(name platform) {
                require_auth(contract);

                platforms plat_table(contract, contract.value);
                auto plat_iter = plat_table.find(platform.value);
                check(plat_iter == plat_table.end(), "identical platform exists");

                plat_table.emplace(contract, [&](auto& row) {
                    row.id = platform;
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
                name platform,
                string url
            ) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(alias));
                check(profile_iter != alias_index.end(), "profile not found");

                name owner = profile_iter->owner;

                require_auth(owner);

                platforms plat_table(contract, contract.value);
                auto plat_iter = plat_table.find(platform.value);
                check(plat_iter != plat_table.end(), "platform not found");

                plat_table.modify(plat_iter, contract, [&](auto& row) {
                    row.counter++;
                });

                string token = vapaee::utils::prng_token(12);

                links link_table(contract, owner.value);
                link_table.emplace(owner, [&](auto& row) {
                    row.link_id = link_table.available_primary_key();
                    row.platform = platform;
                    row.url = url;
                    row.token = token;
                });

                print(token);
            }

        };     
    };
};
