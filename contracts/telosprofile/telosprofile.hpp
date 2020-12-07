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

            ACTION addplatform(string platform) {
                core::action_add_platform(platform);
            }

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

            ACTION witness(
                string witness_alias,
                string link_alias,
                uint64_t link_id
            ) {
                plink::action_witness(witness_alias, link_alias, link_id);
            }

            ACTION updpoints(string alias) {
                prof::action_update_points(alias);
            }

            ACTION updlinkpts(string alias, uint64_t link_id) {
                plink::action_update_link_points(alias, link_id);
            }

            ACTION addorg(string alias, string org_name) {
                org::action_add_organization(alias, org_name);
            }

    };

}; // eosio namespace