#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/modules/core.hpp>

using namespace eosio;
using namespace std;

namespace vapaee {
    using namespace tprofile;

    CONTRACT telosprofile : public eosio::contract {

        private:

#include <vapaee/tprofile/tables.all.hpp>

        public:
            using contract::contract;

            ACTION addplatform(string platform) {
                core::action_add_platform(platform);
            }

            ACTION addprofile(name owner, string alias) {
                core::action_add_profile(owner, alias);
            }

            ACTION chgprofile(string old_alias, string new_alias) {
                core::action_chg_profile(old_alias, new_alias);
            }

            ACTION addlink(
                string alias,
                string platform,
                string url
            ) {
                core::action_add_link(alias, platform, url);
            }

            ACTION chglink(
                string alias,
                uint64_t link_id,
                string url
            ) {
                core::action_chg_link(alias, link_id, url);
            }

            ACTION witness(
                string witness_alias,
                string link_alias,
                uint64_t link_id
            ) {
                core::action_witness(witness_alias, link_alias, link_id);
            }

    };

}; // eosio namespace