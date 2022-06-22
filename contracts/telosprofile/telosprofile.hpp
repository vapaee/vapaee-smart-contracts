#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/base/utils.hpp>
#include <vapaee/tprofile/modules.all.hpp>

using namespace eosio;
using namespace std;

namespace vapaee {
    using namespace tprofile;

    class [[eosio::contract("telosprofile")]] telosprofile : public eosio::contract {
        private:
            using contract::contract;

#include <vapaee/tprofile/tables.all.hpp>
        public:
            telosprofile(name receiver, name code, datastream<const char*> ds) :
                contract(receiver, code, ds)
                { vapaee::current_contract = receiver; }

            // Internal
            [[eosio::action]]
			void addplatform(string platform) {
                core::action_add_platform(platform);
            }

            // Prifile auto management
            [[eosio::action]]
			void addprofile(name owner, string alias) {
                prof::action_add_profile(owner, alias);
            }

            [[eosio::action]]
			void chgprofile(string old_alias, string new_alias) {
                prof::action_chg_profile(old_alias, new_alias);
            }

            [[eosio::action]]
			void addlink(
                string alias,
                string platform,
                string url
            ) {
                plink::action_add_link(alias, platform, url);
            }

            [[eosio::action]]
			void chglink(
                string alias,
                uint64_t link_id,
                string url
            ) {
                plink::action_chg_link(alias, link_id, url);
            }

            // prooflink: after having executed addlink to get a token
            // the user publishes that token in the external platform
            // obtaining the url of the publication.
            // that url is the proof for this link identified by link_id
            [[eosio::action]]
			void prooflink(
                string alias,
                uint64_t link_id,
                string proof_url
            ) {
                plink::action_set_link_proof(alias, link_id, proof_url);
            }

            // witness: profile identified by witness_alias
            // wants to be a WITNESS for profile identified by link_alias
            // and certify that the external platform link identified
            // by link_id has been verified. 
            [[eosio::action]]
			void witness(
                string witness_alias,
                string link_alias,
                uint64_t link_id
            ) {
                plink::action_witness(witness_alias, link_alias, link_id);
            }

            // Maintenance & points consistency 
            [[eosio::action]]
			void updpoints(string alias) {
                prof::action_update_points(alias);
            }

            [[eosio::action]]
			void updlinkpts(string alias, uint64_t link_id) {
                plink::action_update_link_points(alias, link_id);
            }

            [[eosio::action]]
			void grantaccess (
                string alias,
                name target,
                name allow_contract,
                name allow_action
            ) {
                prof::action_grant_access(alias, target, allow_contract, allow_action);
            }

            [[eosio::action]]
			void revokeaccess (
                string alias,
                uint64_t grant_id
            ) {
                prof::action_revoke_access(alias, grant_id);
            }

            // -- Organizations --
            [[eosio::action]]
			void addorg(string creator_alias, string org_name) {
                org::action_add_organization(creator_alias, org_name);
            }
            
            [[eosio::action]]
			void setupprofile(string creator_alias, string org_name, name dapp) {
                org::action_setup_organization_profile(creator_alias, org_name, dapp);
            }

            [[eosio::action]]
			void initasset(string creator_alias, string org_name, name field, asset asset_unit) {
                org::action_init_organization_asset(creator_alias, org_name, field, asset_unit);
            }

            [[eosio::action]]
			void delorg(string creator_alias, string org_name) {
                org::action_del_organization(creator_alias, org_name);
            }

            [[eosio::action]]
			void addmember(
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

            [[eosio::action]]
			void delmember(
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

            [[eosio::action]]
			void addrole(
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

            [[eosio::action]]
			void delrole(
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

            [[eosio::action]]
			void chgasset(
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

    };

}; // eosio namespace
 
