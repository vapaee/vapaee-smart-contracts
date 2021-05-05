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
                auto profile_it = alias_index.find(vapaee::utils::hash(creator_alias));
                check(profile_it != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_it);
                check(owner != "null"_n, "not authorized");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_it = oname_index.find(vapaee::utils::hash(org_name));
                check(org_it == oname_index.end(), "organization exists");

                auto org_profile_it = alias_index.find(vapaee::utils::hash(org_name));
                check(org_profile_it == alias_index.end(), "can't create organization profile");
                prof::action_add_profile(owner, org_name);
                org_profile_it = alias_index.find(vapaee::utils::hash(org_name));

                org_table.emplace(owner, [&](auto& row) {
                    row.id = org_table.available_primary_key();
                    row.org_name = org_name;
                    row.profile = org_profile_it->id;

                    asset zero_valued_asset = asset(0, ORG_EMPTY_SLOT_SYMBOL);
                    row.index1 = zero_valued_asset;
                    row.index2 = zero_valued_asset;
                    row.index3 = zero_valued_asset;
                    row.index4 = zero_valued_asset;
                    row.index5 = zero_valued_asset;

                    members member_table(contract, row.id);
                    member_table.emplace(owner, [&](auto& row) {
                        row.profile_id = profile_it->id;
                        row.roles.push_back(ORG_CREATOR);
                        row.index1 = zero_valued_asset;
                        row.index2 = zero_valued_asset;
                        row.index3 = zero_valued_asset;
                        row.index4 = zero_valued_asset;
                        row.index5 = zero_valued_asset;
                    });
                });
            }

            void action_setup_organization_profile(
                string creator_alias, string org_name, name dapp 
            ) {
                profiles prof_table(contract, contract.value);
                
                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_it = alias_index.find(vapaee::utils::hash(creator_alias));
                check(profile_it != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_it);
                check(owner != "null"_n, "not authorized (sig)");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_it = oname_index.find(vapaee::utils::hash(org_name));
                check(org_it != oname_index.end(), "organization not found");
                
                members member_table(contract, org_it->id);
                auto creator_ms_it = member_table.find(profile_it->id);
                check(creator_ms_it != member_table.end(), "not a member (creator)");
                check(has_role(ORG_CREATOR, creator_ms_it), "not authorized (org)");

                auto org_prof_it = alias_index.find(vapaee::utils::hash(org_name));
                alias_index.modify(org_prof_it, contract, [&](auto& row) {
                    row.contract = dapp;
                    row.owners.push_back(dapp);
                });
            }

            void action_init_organization_asset(
                string creator_alias,
                string org_name,
                name field,
                asset asset_unit
            ) {
                profiles prof_table(contract, contract.value);
                
                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_it = alias_index.find(vapaee::utils::hash(creator_alias));
                check(profile_it != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_it);
                check(owner != "null"_n, "not authorized (sig)");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_it = oname_index.find(vapaee::utils::hash(org_name));
                check(org_it != oname_index.end(), "organization not found");
                
                members member_table(contract, org_it->id);
                auto creator_ms_it = member_table.find(profile_it->id);
                check(creator_ms_it != member_table.end(), "not a member (creator)");
                check(has_role(ORG_CREATOR, creator_ms_it), "not authorized (org)");

                oname_index.modify(org_it, owner, [&](auto& row) {
                    asset zero_valued_asset = asset(0, asset_unit.symbol);
                    switch(field.value) {
                        case name("index1").value:
                            row.index1 = zero_valued_asset;
                            break;
                        case name("index2").value:
                            row.index2 = zero_valued_asset;
                            break;
                        case name("index3").value:
                            row.index3 = zero_valued_asset;
                            break;
                        case name("index4").value:
                            row.index4 = zero_valued_asset;
                            break;
                        case name("index5").value:
                            row.index5 = zero_valued_asset;
                            break;
                        default:
                            check(false, "field must be one of (index[1-5])");
                    }
                });
            }

            void action_del_organization(string creator_alias, string org_name) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto profile_it = alias_index.find(vapaee::utils::hash(creator_alias));
                check(profile_it != alias_index.end(), "profile not found");

                name owner = signed_by_any_owner(profile_it);
                check(owner != "null"_n, "not authorized");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_it = oname_index.find(vapaee::utils::hash(org_name));
                check(org_it != oname_index.end(), "organization not found");
                
                members member_table(contract, org_it->id);
                auto member_it = member_table.begin();
                bool is_creator = member_it->profile_id == profile_it->id;
                member_it++;
                
                check(
                    is_creator && (member_it == member_table.end()),
                    "organization mustn't have members"
                );
                    
                member_it = member_table.begin();
                member_table.erase(member_it);

                oname_index.erase(org_it);
            }

            void action_add_member(
                string admin_alias,
                string org_name,
                string user_alias
            ) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto admin_it = alias_index.find(vapaee::utils::hash(admin_alias));
                check(admin_it != alias_index.end(), "profile not found (admin)");

                auto user_it = alias_index.find(vapaee::utils::hash(user_alias));
                check(user_it != alias_index.end(), "profile not found (user)");

                name owner = signed_by_any_owner(admin_it);
                check(owner != "null"_n, "not authorized (sig)");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_it = oname_index.find(vapaee::utils::hash(org_name));
                check(org_it != oname_index.end(), "organization not found");
                
                members member_table(contract, org_it->id);
                auto admin_ms_it = member_table.find(admin_it->id);
                check(admin_ms_it != member_table.end(), "not a member (admin)");
                check(
                    has_role(ORG_CREATOR, admin_ms_it) ||
                    has_role(ORG_ADMINISTRATOR, admin_ms_it), 
                    "not authorized (org)"
                );

                auto user_ms_it = member_table.find(user_it->id);
                check(user_ms_it == member_table.end(), "already a member");

                member_table.emplace(owner, [&](auto& row) {
                    row.profile_id = user_it->id;
                    row.index1 = asset(0, org_it->index1.symbol);
                    row.index2 = asset(0, org_it->index2.symbol);
                    row.index3 = asset(0, org_it->index3.symbol);
                    row.index4 = asset(0, org_it->index4.symbol);
                    row.index5 = asset(0, org_it->index5.symbol);
                });

            }

            void action_del_member(
                string admin_alias,
                string org_name,
                string user_alias
            ) {
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto admin_it = alias_index.find(vapaee::utils::hash(admin_alias));
                check(admin_it != alias_index.end(), "profile not found (admin)");

                auto user_it = alias_index.find(vapaee::utils::hash(user_alias));
                check(user_it != alias_index.end(), "profile not found (user)");

                name owner = signed_by_any_owner(admin_it);
                check(owner != "null"_n, "not authorized (sig)");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_it = oname_index.find(vapaee::utils::hash(org_name));
                check(org_it != oname_index.end(), "organization not found");
                
                members member_table(contract, org_it->id);
                auto admin_ms_it = member_table.find(admin_it->id);
                check(admin_ms_it != member_table.end(), "not a member (admin)");
                check(
                    has_role(ORG_CREATOR, admin_ms_it) ||
                    has_role(ORG_ADMINISTRATOR, admin_ms_it), 
                    "not authorized (org)"
                );

                auto user_ms_it = member_table.find(user_it->id);
                check(user_ms_it != member_table.end(), "not a member (user)");
                check(!has_role(ORG_CREATOR, user_ms_it), "can't delete creator");
                
                member_table.erase(user_ms_it);
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
                auto admin_it = alias_index.find(vapaee::utils::hash(admin_alias));
                check(admin_it != alias_index.end(), "profile not found (admin)");

                auto user_it = alias_index.find(vapaee::utils::hash(user_alias));
                check(user_it != alias_index.end(), "profile not found (user)");

                name owner = signed_by_any_owner(admin_it);
                check(owner != "null"_n, "not authorized (sig)");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_it = oname_index.find(vapaee::utils::hash(org_name));
                check(org_it != oname_index.end(), "organization not found");
                
                members member_table(contract, org_it->id);
                auto admin_ms_it = member_table.find(admin_it->id);
                check(admin_ms_it != member_table.end(), "not a member (admin)");
                bool is_creator = has_role(ORG_CREATOR, admin_ms_it);
                check(
                    is_creator ||
                    has_role(ORG_ADMINISTRATOR, admin_ms_it), 
                    "not authorized (org)"
                );

                auto user_ms_it = member_table.find(user_it->id);
                check(user_ms_it != member_table.end(), "not a member (user)");

                // only creator can modify creator
                if (has_role(ORG_CREATOR, user_ms_it))
                    check(is_creator, "creator permission required");

                check(
                    !has_role(role_name, user_ms_it),
                    "user has the role"
                );

                member_table.modify(user_ms_it, owner, [&](auto& row) {
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
                auto admin_it = alias_index.find(vapaee::utils::hash(admin_alias));
                check(admin_it != alias_index.end(), "profile not found (admin)");

                auto user_it = alias_index.find(vapaee::utils::hash(user_alias));
                check(user_it != alias_index.end(), "profile not found (user)");

                name owner = signed_by_any_owner(admin_it);
                check(owner != "null"_n, "not authorized (sig)");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_it = oname_index.find(vapaee::utils::hash(org_name));
                check(org_it != oname_index.end(), "organization not found");
                
                members member_table(contract, org_it->id);
                auto admin_ms_it = member_table.find(admin_it->id);
                check(admin_ms_it != member_table.end(), "not a member (admin)");
                bool is_creator = has_role(ORG_CREATOR, admin_ms_it);
                check(
                    is_creator || has_role(ORG_ADMINISTRATOR, admin_ms_it), 
                    "not authorized (org)"
                );

                auto user_ms_it = member_table.find(user_it->id);
                check(user_ms_it != member_table.end(), "not a member (user)");

                // only creator can modify creator
                if (has_role(ORG_CREATOR, user_ms_it))
                    check(is_creator, "creator permission required");
                
                check(
                    has_role(role_name, user_ms_it),
                    "user doesn't have the role"
                );

                member_table.modify(user_ms_it, owner, [&](auto& row) {
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
                name field,         // index[1-5] 
                name action,        // add remove
                asset quantity,
                string user_alias
            ) { 
                profiles prof_table(contract, contract.value);

                auto alias_index = prof_table.get_index<"alias"_n>();
                auto admin_it = alias_index.find(vapaee::utils::hash(admin_alias));
                check(admin_it != alias_index.end(), "profile not found (admin)");

                auto user_it = alias_index.find(vapaee::utils::hash(user_alias));
                check(user_it != alias_index.end(), "profile not found (user)");

                name owner = signed_by_any_owner(admin_it);
                check(owner != "null"_n, "not authorized (sig)");

                organizations org_table(contract, contract.value);
                auto oname_index = org_table.get_index<"orgname"_n>();
                auto org_it = oname_index.find(vapaee::utils::hash(org_name));
                check(org_it != oname_index.end(), "organization not found");
                
                members member_table(contract, org_it->id);
                auto admin_ms_it = member_table.find(admin_it->id);
                check(admin_ms_it != member_table.end(), "not a member (admin)");
                bool is_creator = has_role(ORG_CREATOR, admin_ms_it);
                check(
                    is_creator || has_role(ORG_ADMINISTRATOR, admin_ms_it), 
                    "not authorized (org)"
                );

                auto user_ms_it = member_table.find(user_it->id);
                check(user_ms_it != member_table.end(), "not a member (user)");

                // only creator can modify creator
                if (has_role(ORG_CREATOR, user_ms_it))
                    check(is_creator, "creator permission required");

                check((action == "add"_n) || (action == "sub"_n), "invalid operator");
                bool is_addition = action == "add"_n;

                if (!is_addition)
                    quantity *= -1;

                member_table.modify(user_ms_it, contract, [&](auto& row) {
                    switch(field.value) {
                        case "index1"_n.value:
                            row.index1 += quantity;
                            break;
                        case "index2"_n.value:
                            row.index2 += quantity;
                            break;
                        case "index3"_n.value:
                            row.index3 += quantity;
                            break;
                        case "index4"_n.value:
                            row.index4 += quantity;
                            break;
                        case "index5"_n.value:
                            row.index5 += quantity;
                            break;
                        default:
                            check(false, "field must be one of (index[1-5])");
                    }
                });
            }

        }
    }
};
