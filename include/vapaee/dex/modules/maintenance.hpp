#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/constants.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/record.hpp>
#include <vapaee/dex/modules/global.hpp>
#include <vapaee/dex/modules/experience.hpp>
#include <vapaee/book/modules/exchange.hpp>

namespace vapaee {
    namespace dex {

        using namespace utils;

        namespace maintenance {


            void aux_maintenance_cancel_sell_order(
                    name owner,
                    const symbol_code & token_a,
                    const symbol_code & token_p,
                    uint64_t order_id,
                    uint64_t market_id
                )
                {

                uint64_t can_market = market_id;
                if (market_id % 2 != 0) {
                    can_market--;
                }

                // init vector with market_id as the only entry
                std::vector<uint64_t> orders;
                orders.push_back(market_id);

                // determine type of order (buy or sell) by comparng market_id with can_market
                name type = (market_id == can_market) ? name("sell") : name("buy");

                action(
                    permission_level{contract,name("active")},
                    contract,
                    name("cancel"),
                    std::make_tuple(owner, type, token_a, token_p, orders)
                ).send();

            }

            string aux_maintenance_from_delmarkets(asset& pts, asset& exp) {
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_delmarkets()\n");
                PRINT(" pts: ", pts.to_string(), "\n");
                PRINT(" exp: ", exp.to_string(), "\n");
               
                state gconf = vapaee::dex::global::get(); 

                string report;
                delmarkets deltable(contract, contract.value);
                auto delptr = deltable.begin();
                bool sellorders_clean = false;
                if (delptr != deltable.end()) {
                    uint64_t market_id = delptr->id;

                    // delete de first sellorders order
                    sellorders stable(contract, market_id);
                    auto sptr = stable.begin();
                    if (sptr != stable.end()) {
                        report += string("|delmarket-cancel-order:")+ std::to_string((unsigned long)sptr->id)  + "," + std::to_string((unsigned long)market_id);
                        PRINT("  => cancel_sell_order(): ",sptr->owner.to_string(), " id: ", std::to_string((unsigned long)sptr->id), " market: ", std::to_string((unsigned long)market_id), "\n"); 
                        aux_maintenance_cancel_sell_order(sptr->owner, sptr->price.symbol.code(), sptr->inverse.symbol.code(), sptr->id, market_id);
                        exp.amount += 5 * gconf.maint_reward_delmarkets_exp;
                        pts.amount += 5 * gconf.maint_reward_delmarkets_pts;
                    } else {
                        PRINT(" -> sellorders_clean \n");
                        sellorders_clean = true;
                    }

                    if (sellorders_clean) {
                        PRINT("  => delmarkets.erase(): ",std::to_string((unsigned long)delptr->id), " DELETE delmarket!!!!\n");
                        report += string("|delmarket-erase-market:") + std::to_string((unsigned long)market_id);
                        deltable.erase(*delptr);
                        exp.amount += 3 * gconf.maint_reward_delmarkets_exp;
                        pts.amount += 5 * gconf.maint_reward_delmarkets_pts;
                    }

                }
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_delmarkets() ...\n");                
                return report;
            }

            string aux_maintenance_from_points(asset& pts, asset& exp) {
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_points()\n");
                PRINT(" pts: ", pts.to_string(), "\n");
                PRINT(" exp: ", exp.to_string(), "\n");

                state gconf = vapaee::dex::global::get(); 

                string report;
                int pprune_weeks = vapaee::dex::global::get().pprune;
                uint32_t current_week = vapaee::dex::global::get_current_week_number();
                uint32_t expire_week = current_week - pprune_weeks;

                PRINT(" -> current_week:  ",std::to_string((unsigned long)current_week)," \n");
                PRINT(" -> expire_week:   ",std::to_string((unsigned long)expire_week)," \n");

                points ptable(contract, contract.value);
                int max = 3;
                int count = 0;
                for (auto ptr = ptable.begin(); ptr != ptable.end() && count < max; ptr = ptable.begin()) {
                    if (ptr == ptable.end()) {
                        break;
                    } else {
                        if (ptr->week < expire_week) {
                            count++;
                            PRINT(" => points.erase(): ",std::to_string((unsigned long)ptr->id)," '", ptr->owner.to_string(), "' \n");
                            ptable.erase(*ptr);
                        } else {
                            PRINT(" -> not expired: ",std::to_string((unsigned long)ptr->id)," '", ptr->owner.to_string(), "' \n");
                            break;
                        }
                    }
                }
                if (count>0) {
                    report += string("|points-prune:") + std::to_string(count);
                    exp.amount += count * gconf.maint_reward_points_exp;
                    pts.amount += count * gconf.maint_reward_points_pts;
                }

                PRINT("vapaee::dex::maintenance::aux_maintenance_from_points() ...\n");
                return report;
            }

            string aux_maintenance_from_ballots(asset& pts, asset& exp) {
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_ballots()\n");
                PRINT(" pts: ", pts.to_string(), "\n");
                PRINT(" exp: ", exp.to_string(), "\n");

                state gconf = vapaee::dex::global::get(); 
                
                string report;
                int bprune = vapaee::dex::global::get().bprune;
                PRINT(" -> bprune: ",bprune," \n");

                ballots balltable(contract, contract.value);
                auto ptr = balltable.begin();

                time_point_sec _now = vapaee::dex::global::get_now_time_point_sec();

                if (ptr != balltable.end()) {
                    PRINT(" -> ptr->num: ",std::to_string((unsigned long)ptr->id)," \n");
                    uint64_t next_id = balltable.available_primary_key();
                    PRINT(" -> next_id: ",std::to_string((unsigned long)next_id)," \n");
                    uint64_t dif = next_id - ptr->id;
                    PRINT(" -> dif: ",std::to_string((unsigned long)dif)," \n");
                    if (dif > bprune) {
                        // discard ballot if not finished
                        PRINT(" -> _now:          ",std::to_string((unsigned long)_now.utc_seconds)," \n");
                        PRINT(" -> itr->finished: ",std::to_string((unsigned long)ptr->finished.utc_seconds)," \n");
                        if (ptr->finished < _now) {
                            report += string("|ballots-prune:") + ptr->ballot_name.to_string();
                            PRINT("  => ballots.erase(): ",std::to_string((unsigned long)ptr->id), "\n");
                            balltable.erase(*ptr);
                            exp.amount += 5 * gconf.maint_reward_ballots_exp;
                            pts.amount += 10 * gconf.maint_reward_ballots_pts;                   
                        } else {
                            PRINT(" -> we skip ballot because is not finished: ",std::to_string((unsigned long)ptr->id)," \n");         
                        }
                    }
                }

                PRINT("vapaee::dex::maintenance::aux_maintenance_from_ballots() ...\n");
                return report;
            }

            void action_do_maintenance(name credits_to) {
                PRINT("vapaee::dex::maintenance::action_do_maintenance() ------------------------------\n");
                PRINT(" credits_to: ",credits_to.to_string()," \n");

                string report;
                asset points = asset(0, vapaee::dex::experience::POINTS_SYMBOL);
                asset exp = asset(0, vapaee::dex::experience::EXP_SYMBOL);
                PRINT("       --------------------------------\n");
                PRINT("       -- MAINTENANCE: delmarkets    --\n");
                report += aux_maintenance_from_delmarkets(points, exp);
                
                PRINT("       --------------------------------\n");
                PRINT("       -- MAINTENANCE: ballots       --\n");
                report += aux_maintenance_from_ballots(points, exp);

                PRINT("       --------------------------------\n");
                PRINT("       -- MAINTENANCE: weekly points --\n");
                report += aux_maintenance_from_points(points, exp);

                if (points.amount > 0) {
                    action(
                        permission_level{contract,name("active")},
                        vapaee::dex::contract,
                        name("reward"),
                        std::make_tuple(credits_to, points, exp)
                    ).send();
                }

                PRINT("vapaee::dex::maintenance::action_do_maintenance() ------------------------------ (end)\n");
            }
        };     
    };
};
