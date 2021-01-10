#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/ttracker/modules.all.hpp>

using namespace eosio;
using namespace std;

namespace vapaee {
    using namespace ttracker;

    CONTRACT eventtracker : public eosio::contract {
        public:
            using contract::contract;

#include <vapaee/ttracker/tables.all.hpp>
            // admin
            ACTION addorg(org) {}

            // organizations
            ACTION create_map(org, title, config) {}
            ACTION create_dome(org, title, config, start, finish) {}
            ACTION create_target(org, title, config, dome) {}
            ACTION geo_event(org, user_alias, target, dome, map, lat, lng) {}

            

    };

}; // eosio namespace
