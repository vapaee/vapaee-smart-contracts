#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/tables.hpp>
#include <vapaee/tprofile/modules/prof.hpp>

using namespace vapaee::tprofile::prof;  // signed_by_any_owner

namespace vapaee {
    namespace tprofile {
        namespace org {

            static name SUB_CA_ROLE = "subcertauth"_n;

            template <typename prof_iter_t, typename org_iter_t>
            bool is_certificate_authority(prof_iter_t& prof_iter, org_iter_t& org_iter) {
                return org_iter->members[0] == prof_iter->id;
            }

            template <typename prof_iter_t, typename org_iter_t>
            bool is_member(prof_iter_t& prof_iter, org_iter_t& org_iter) {
                bool found = false;
                for(auto pid : org_iter->members) {
                    if (pid == prof_iter->id) {
                        found = true;
                        break;
                    }
                }
                return found;
            }

            template <typename prof_iter_t>
            bool has_role(name role, prof_iter_t& prof_iter, roles& role_table) {
                auto role_index = role_table.get_index<"role"_n>();
                auto role_iter = role_index.find(role.value);
                while (role_iter != role_index.end()) {
                    if (role_iter->profile_id == prof_iter->id)
                        return true;
                }
                return false;
            }

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

            void action_add_role(
                string auth_alias,
                string org_name,
                name role_name,
                string member_alias
            ) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(auth_alias));
                check(profile_iter != alias_index.end(), "profile not found");

                auto member_iter = alias_index.find(vapaee::utils::hash(member_alias));
                check(member_iter != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner<decltype(profile_iter)>(profile_iter);
                check(owner != "null"_n, "not authorized (sig)");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_iter = oname_index.find(vapaee::utils::hash(org_name));
                check(org_iter != oname_index.end(), "organization not found");
                roles role_table(contract, org_iter->id);
                check(
                    is_certificate_authority<decltype(profile_iter), decltype(org_iter)>(profile_iter, org_iter) ||
                    has_role<decltype(profile_iter)>(SUB_CA_ROLE, profile_iter, role_table), 
                    "not authorized (org)"
                );

                if (!is_member<decltype(member_iter), decltype(org_iter)>(member_iter, org_iter))
                    oname_index.modify(org_iter, owner, [&](auto& row) {
                        row.members.push_back(member_iter->id);
                    });

                role_table.emplace(owner, [&](auto& row) {
                    row.id = org_table.available_primary_key();
                    row.profile_id = member_iter->id;
                    row.role_name = role_name;
                });
            }

        }
    }
};