#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/base/dispatcher.hpp>
#include <vapaee/profile/modules/core.hpp>

using namespace eosio;
using namespace std;

namespace vapaee {
    using namespace tprofile;

    CONTRACT telosprofile : public eosio::contract {
    
        private:

#include <vapaee/profile/tables.all.hpp>

        public:
            using contract::contract;

        public:

            ACTION addprofile(name owner, std::string alias) {
                core::action_add_profile(owner, alias);
            }

            ACTION purgeprofile(std::string alias) {
                core::action_purge_profile(alias);
            }

    };

}; // eosio namespace