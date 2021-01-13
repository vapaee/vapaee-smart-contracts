#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/tables.hpp>
#include <vapaee/tprofile/modules/prof.hpp>

using namespace vapaee::tprofile::prof;  // signed_by_any_owner

#define EMPTY_SLOT_SYMBOL 

namespace vapaee {
    namespace tprofile {
        namespace org {

            static symbol ORG_EMPTY_SLOT_SYMBOL = symbol(symbol_code("UNUSED"), 0);
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

                auto org_profile_iter = alias_index.find(vapaee::utils::hash(org_name));
                check(org_profile_iter == alias_index.end(), "can't create organization profile");
                prof::action_add_profile(owner, org_name);
                org_profile_iter = alias_index.find(vapaee::utils::hash(org_name));

                org_table.emplace(owner, [&](auto& row) {
                    row.id = org_table.available_primary_key();
                    row.org_name = org_name;
                    row.profile = org_profile_iter->id;

                    asset zero_valued_asset = asset(0, ORG_EMPTY_SLOT_SYMBOL);
                    row.points = zero_valued_asset;
                    row.credits = zero_valued_asset;
                    row.rewards = zero_valued_asset;
                    row.trust = zero_valued_asset;
                    row.rep = zero_valued_asset;

                    members member_table(contract, row.id);
                    member_table.emplace(owner, [&](auto& row) {
                        row.profile_id = profile_iter->id;
                        row.roles.push_back(ORG_CREATOR);
                    });
                });
            }

            void action_setup_organization_profile(string creator_alias, string org_name, name contract) {
                // TODO:
                /*
                    profile app = create_new_app_profile_for_organization(creator_alias, org_name);
                    app.contract = contract;

                    ort org_table = get_org_table();
                    auto org_itr = org_table.find(org_name);

                    org_itr.modify({
                        row.app = app.id;
                    });
                */
            }

            void action_init_organization_asset(
                string creator_alias,
                string org_name,
                name field,
                asset asset_unit
            ) {
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

                auto org_iter_id = org_table.find(org_iter->id);

                org_table.modify(org_iter_id, owner, [&](auto& row) {
                    asset zero_valued_asset = asset(0, asset_unit.symbol);
                    switch(field.value) {
                        case name("points").value:
                            row.points = zero_valued_asset;
                            break;
                        case name("credits").value:
                            row.credits = zero_valued_asset;
                            break;
                        case name("rewards").value:
                            row.rewards = zero_valued_asset;
                            break;
                        case name("trust").value:
                            row.trust = zero_valued_asset;
                            break;
                        case name("rep").value:
                            row.rep = zero_valued_asset;
                            break;
                        default:
                            check(false, "field must be one of (points credits rewards trust rep)");
                    }
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

                    row.points     = asset(0, org_iter->points.symbol);
                    row.credits    = asset(0, org_iter->credits.symbol);
                    row.rewards    = asset(0, org_iter->rewards.symbol);
                    row.trust      = asset(0, org_iter->trust.symbol);
                    row.rep        = asset(0, org_iter->rep.symbol);
                    
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

            void action_change_member_asset(
                string admin_alias,
                string org_name,
                name field,         // points credits rewards trust rep 
                name action,        // add remove
                asset quantity,
                string user_alias
            ) {
                // TODO:
                /*

                // pseudocode:
                member = organizatio.get_member(user_alias);
                if (action == "add") {
                    member[field] += quantity;
                } else if (action == "remove") {
                    member[field] -= quantity;
                }
                
                */
            }


            void action_grant_access(
                string user_alias,
                string org_name,
                name contract,
                name action
            ) {
                // TODO
                /*
                name contract = get_organization_smart_contract(org_name)
                access table(self, get_profile_id(user_alias))
                table.emplace({
                    row.organization = org_name;
                    row.granted = contract;
                    row.contract = contract;
                    row.action = action;
                })
                */
            }

            void action_revoke_access(string user_alias, uint64_t grant_id) {
                // TODO
                /*
                access table(self, get_profile_id(user_alias))
                iter = table.find(grant_id)
                table.erases(iter)
                */
            }

        }
    }
};
