#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/modules.all.hpp>

using namespace eosio;
using namespace std;

namespace vapaee {
    using namespace tprofile;

    CONTRACT telosprofile : public eosio::contract {
        public:
            using contract::contract;

#include <vapaee/tprofile/tables.all.hpp>

            // Internal
            ACTION addplatform(string platform) {
                core::action_add_platform(platform);
            }

            // Prifile auto management
            ACTION addprofile(name owner, string alias) {
                prof::action_add_profile(owner, alias);
            }

            ACTION chgprofile(string old_alias, string new_alias) {
                prof::action_chg_profile(old_alias, new_alias);
            }

            ACTION addlink(
                string alias,
                string platform,
                string url
            ) {
                plink::action_add_link(alias, platform, url);
            }

            ACTION chglink(
                string alias,
                uint64_t link_id,
                string url
            ) {
                plink::action_chg_link(alias, link_id, url);
            }

            // ACTION prooflink: after having executed addlink to get a token
            // the user publishes that token in the external platform
            // obtaining the url of the publication.
            // that url is the proof for this link identified by link_id
            ACTION prooflink(
                string alias,
                uint64_t link_id,
                string proof_url
            ) {
                plink::action_set_link_proof(alias, link_id, proof_url);
            }

            // ACTION witness: profile identified by witness_alias
            // wants to be a WITNESS for profile identified by link_alias
            // and certify that the external platform link identified
            // by link_id has been verified. 
            ACTION witness(
                string witness_alias,
                string link_alias,
                uint64_t link_id
            ) {
                plink::action_witness(witness_alias, link_alias, link_id);
            }

            // Maintenance & points consistency 
            ACTION updpoints(string alias) {
                prof::action_update_points(alias);
            }

            ACTION updlinkpts(string alias, uint64_t link_id) {
                plink::action_update_link_points(alias, link_id);
            }

            // -- Organizations --
            ACTION addorg(string creator_alias, string org_name) {
                org::action_add_organization(creator_alias, org_name);
            }

            ACTION orgasset(string creator_alias, string org_name, name field, asset asset_unit) {
                org::action_add_organization_asset(creator_alias, org_name, field, asset_unit);
            }

            ACTION delorg(string creator_alias, string org_name) {
                org::action_del_organization(creator_alias, org_name);
            }

            ACTION addmember(
                string admin_alias,
                string org_name,
                string user_alias
            ) {
                org::action_add_member(
                    admin_alias,
                    org_name,
                    user_alias
                );
            }

            ACTION delmember(
                string admin_alias,
                string org_name,
                string user_alias
            ) {
                org::action_del_member(
                    admin_alias,
                    org_name,
                    user_alias
                );
            }

            ACTION addrole(
                string admin_alias,
                string org_name,
                name role_name,
                string user_alias
            ) {
                org::action_add_role(
                    admin_alias,
                    org_name,
                    role_name,
                    user_alias
                );
            }

            ACTION delrole(
                string admin_alias,
                string org_name,
                name role_name,
                string user_alias
            ) {
                org::action_del_role(
                    admin_alias,
                    org_name,
                    role_name,
                    user_alias
                );
            }

            ACTION chgasset(
                string admin_alias,
                string org_name,
                name field,        // points credits rewards trust rep 
                name action,       // add remove
                asset quantity,
                string user_alias
            ) {
                org::action_change_member_asset(
                    admin_alias,
                    org_name,
                    field,
                    action,
                    quantity,
                    user_alias
                );
            }


            ACTION grantaccess (
                string user_alias,
                string org_name,
                name contract,
                name action
            ) {
                org::action_grant_access(user_alias, org_name, contract, action);
            }

            ACTION revokaccess (
                string user_alias,
                uint64_t grant_id
            ) {
                org::action_revoke_access(user_alias, grant_id);
            }


    };

}; // eosio namespace
 