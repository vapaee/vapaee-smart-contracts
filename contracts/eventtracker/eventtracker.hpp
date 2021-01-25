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

            // map
            ACTION addmap(
                string alias,
                string title,
                string config
            ) {
                map::action_add_map(alias, title, config);
            }

            // season
            ACTION addseason(
                string alias,
                string title,
                time_point_sec start,
                time_point_sec finish,
                string config
            ){
                season::action_add_season(alias, title, start, finish, config);
            }

            // add to target creator whitelist
            ACTION addtcwlist(
                string alias,
                string season,
                string new_alias
            ){
                season::action_add_target_creator_whitelist(alias, season, new_alias);
            }

            // target
            ACTION addtarget(
                string alias,
                string season,
                string title,
                string config,
                string map,
                asset lat,
                asset lng
            ) {
                target::action_add_target(
                    alias,
                    season,
                    title,
                    config,
                    map,
                    lat,
                    lng
                );
            }

            // add to target whitelist (event creators)
            ACTION addwlist(
                string alias,
                string season,
                string target,
                string new_alias
            ) {
                target::action_add_whitelist(
                    alias,
                    season,
                    target,
                    new_alias
                );
            }

            // event
            ACTION addevent(
                string alias,
                string season,
                string target,
                string map,
                asset lat,
                asset lng,
                string geometry,
                string config
            ) {
                event::action_add_event(
                    alias,
                    season,
                    target,
                    map,
                    lat,
                    lng,
                    geometry,
                    config
                );
            }

    };

}; // eosio namespace
