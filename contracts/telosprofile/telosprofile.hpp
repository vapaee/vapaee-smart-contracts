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

            ACTION addprofile(name owner, std::string alias) {
                core::action_add_profile(owner, alias);
            }

            ACTION purgeprofile(std::string alias) {
                core::action_purge_profile(alias);
            }

            ACTION addlink(
                string alias,
                name platform,
                string url
            ) {
                core::action_add_link(alias, platform, url);
            }

    };

}; // eosio namespace