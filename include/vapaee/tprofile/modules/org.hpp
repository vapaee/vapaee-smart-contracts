#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/tables.hpp>
#include <vapaee/tprofile/modules/prof.hpp>

using namespace vapaee::tprofile::prof;  // signed_by_any_owner

namespace vapaee {
    namespace tprofile {
        namespace org {

            static name ORG_CREATOR = "creator"_n;
            static name ORG_ADMINISTRATOR = "admin"_n;

            template<typename T>
            bool has_role(name role, T& mem_it) {
                return find(
                    mem_it->roles.begin(),
                    mem_it->roles.end(),
                    role
                ) != mem_it->roles.end();
            }

            void action_add_organization(string creator_alias, string org_name) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(creator_alias));
                check(profile_iter != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_iter);
                check(owner != "null"_n, "not authorized");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_iter = oname_index.find(vapaee::utils::hash(org_name));
                check(org_iter == oname_index.end(), "organization exists");

                org_table.emplace(owner, [&](auto& row) {
                    row.id = org_table.available_primary_key();
                    row.org_name = org_name;

                    members member_table(contract, row.id);
                    member_table.emplace(owner, [&](auto& row) {
                        row.profile_id = profile_iter->id;
                        row.roles.push_back(ORG_CREATOR);
                    });
                });
            }

            void action_del_organization(string creator_alias, string org_name) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_iter = alias_index.find(vapaee::utils::hash(creator_alias));
                check(profile_iter != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_iter);
                check(owner != "null"_n, "not authorized");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_iter = oname_index.find(vapaee::utils::hash(org_name));
                check(org_iter != oname_index.end(), "organization not found");
                
                members member_table(contract, org_iter->id);
                auto member_iter = member_table.begin();
                bool is_creator = member_iter->profile_id == profile_iter->id;
                member_iter++;
                
                check(
                    is_creator && (member_iter == member_table.end()),
                    "organization mustn't have members"
                );
                    
                member_iter = member_table.begin();
                member_table.erase(member_iter);

                oname_index.erase(org_iter);
            }

            void action_add_member(
                string admin_alias,
                string org_name,
                string user_alias
            ) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto admin_iter = alias_index.find(vapaee::utils::hash(admin_alias));
                check(admin_iter != alias_index.end(), "profile not found (admin)");

                auto user_iter = alias_index.find(vapaee::utils::hash(user_alias));
                check(user_iter != alias_index.end(), "profile not found (user)");

                name owner = signed_by_any_owner(admin_iter);
                check(owner != "null"_n, "not authorized (sig)");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_iter = oname_index.find(vapaee::utils::hash(org_name));
                check(org_iter != oname_index.end(), "organization not found");
                
                members member_table(contract, org_iter->id);
                auto admin_ms_iter = member_table.find(admin_iter->id);
                check(admin_ms_iter != member_table.end(), "not a member (admin)");
                check(
                    has_role(ORG_CREATOR, admin_ms_iter) ||
                    has_role(ORG_ADMINISTRATOR, admin_ms_iter), 
                    "not authorized (org)"
                );

                auto user_ms_iter = member_table.find(user_iter->id);
                check(user_ms_iter == member_table.end(), "already a member");

                member_table.emplace(owner, [&](auto& row) {
                    row.profile_id = user_iter->id;
                });

            }

            void action_del_member(
                string admin_alias,
                string org_name,
                string user_alias
            ) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto admin_iter = alias_index.find(vapaee::utils::hash(admin_alias));
                check(admin_iter != alias_index.end(), "profile not found (admin)");

                auto user_iter = alias_index.find(vapaee::utils::hash(user_alias));
                check(user_iter != alias_index.end(), "profile not found (user)");

                name owner = signed_by_any_owner(admin_iter);
                check(owner != "null"_n, "not authorized (sig)");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_iter = oname_index.find(vapaee::utils::hash(org_name));
                check(org_iter != oname_index.end(), "organization not found");
                
                members member_table(contract, org_iter->id);
                auto admin_ms_iter = member_table.find(admin_iter->id);
                check(admin_ms_iter != member_table.end(), "not a member (admin)");
                check(
                    has_role(ORG_CREATOR, admin_ms_iter) ||
                    has_role(ORG_ADMINISTRATOR, admin_ms_iter), 
                    "not authorized (org)"
                );

                auto user_ms_iter = member_table.find(user_iter->id);
                check(user_ms_iter != member_table.end(), "not a member (user)");
                check(!has_role(ORG_CREATOR, user_ms_iter), "can't delete creator");
                
                member_table.erase(user_ms_iter);
            }

            void action_add_role(
                string admin_alias,
                string org_name,
                name role_name,
                string user_alias
            ) {
                check(role_name != ORG_CREATOR, "can't give that role");

                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto admin_iter = alias_index.find(vapaee::utils::hash(admin_alias));
                check(admin_iter != alias_index.end(), "profile not found (admin)");

                auto user_iter = alias_index.find(vapaee::utils::hash(user_alias));
                check(user_iter != alias_index.end(), "profile not found (user)");

                name owner = signed_by_any_owner(admin_iter);
                check(owner != "null"_n, "not authorized (sig)");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_iter = oname_index.find(vapaee::utils::hash(org_name));
                check(org_iter != oname_index.end(), "organization not found");
                
                members member_table(contract, org_iter->id);
                auto admin_ms_iter = member_table.find(admin_iter->id);
                check(admin_ms_iter != member_table.end(), "not a member (admin)");
                bool is_creator = has_role(ORG_CREATOR, admin_ms_iter);
                check(
                    is_creator ||
                    has_role(ORG_ADMINISTRATOR, admin_ms_iter), 
                    "not authorized (org)"
                );

                auto user_ms_iter = member_table.find(user_iter->id);
                check(user_ms_iter != member_table.end(), "not a member (user)");

                // only creator can modify creator
                if (has_role(ORG_CREATOR, user_ms_iter))
                    check(is_creator, "creator permission required");

                check(
                    !has_role(role_name, user_ms_iter),
                    "user has the role"
                );

                member_table.modify(user_ms_iter, owner, [&](auto& row) {
                    row.roles.push_back(role_name);
                });
            }

            void action_del_role(
                string admin_alias,
                string org_name,
                name role_name,
                string user_alias
            ) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto admin_iter = alias_index.find(vapaee::utils::hash(admin_alias));
                check(admin_iter != alias_index.end(), "profile not found (admin)");

                auto user_iter = alias_index.find(vapaee::utils::hash(user_alias));
                check(user_iter != alias_index.end(), "profile not found (user)");

                name owner = signed_by_any_owner(admin_iter);
                check(owner != "null"_n, "not authorized (sig)");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_iter = oname_index.find(vapaee::utils::hash(org_name));
                check(org_iter != oname_index.end(), "organization not found");
                
                members member_table(contract, org_iter->id);
                auto admin_ms_iter = member_table.find(admin_iter->id);
                check(admin_ms_iter != member_table.end(), "not a member (admin)");
                bool is_creator = has_role(ORG_CREATOR, admin_ms_iter);
                check(
                    is_creator || has_role(ORG_ADMINISTRATOR, admin_ms_iter), 
                    "not authorized (org)"
                );

                auto user_ms_iter = member_table.find(user_iter->id);
                check(user_ms_iter != member_table.end(), "not a member (user)");

                // only creator can modify creator
                if (has_role(ORG_CREATOR, user_ms_iter))
                    check(is_creator, "creator permission required");
                
                check(
                    has_role(role_name, user_ms_iter),
                    "user doesn't have the role"
                );

                member_table.modify(user_ms_iter, owner, [&](auto& row) {
                    row.roles.erase(
                        remove(
                            row.roles.begin(),
                            row.roles.end(),
                            role_name
                        ),
                        row.roles.end()
                    );
                });
            }

        }
    }
};
