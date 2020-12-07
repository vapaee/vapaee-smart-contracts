#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/tables.hpp>
#include <vapaee/tprofile/modules/prof.hpp>

using namespace vapaee::tprofile::prof;  // signed_by_any_owner

namespace vapaee {
    namespace tprofile {
        namespace org {

            void action_add_organization(string alias, string org_name) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(alias));
                check(profile_iter != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner<decltype(profile_iter)>(profile_iter);
                check(owner != "null"_n, "not authorized");

                organizations org_table(contract, contract.value);
                org_table.emplace(owner, [&](auto& row) {
                    row.id = org_table.available_primary_key();
                    row.org_name = org_name;
                    row.members.push_back(profile_iter->id);
                });
            }

        }
    }
};