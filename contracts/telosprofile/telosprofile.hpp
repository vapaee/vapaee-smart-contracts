#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/modules/core.hpp>

using namespace eosio;
using namespace std;

namespace vapaee {
    using namespace tprofile;

    CONTRACT telosprofile : public eosio::contract {

        /* TODO:
            global:
                * link platform counter:
                  keeps track of how many links are registered
                  for each platform globaly

            profile:
                * link list
                  each link contains:
                        * platform score

                        * witness list
                          witnesses are other profiles that certify that
                          a link is valid

                * KYC level
                  0 - 20, 0 means noob, 20 is celebrity status

                * raw elo
                  1+, this is the raw number used to calculate
                  the KYC level, its calculated using a weighted average.
    
                * witness slots
                  5+, this are slots where users can select from their
                  link witnesses profiles with the highest elo score.

                  each profile raw elo is a weighted average from their
                  witness slots
        */

    
        private:

#include <vapaee/tprofile/tables.all.hpp>

        public:
            using contract::contract;

            ACTION addplatform(name platform) {
                core::action_add_platform(platform);
            }

            ACTION addprofile(name owner, string alias) {
                core::action_add_profile(owner, alias);
            }

            ACTION purgeprofile(string alias) {
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