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

            ACTION addlink(
                string alias,
                string platform,
                string url
            ) {
                core::action_add_link(alias, platform, url);
            }

    };

}; // eosio namespace