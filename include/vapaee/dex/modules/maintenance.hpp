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
                bool history_clean = false;
                bool sellorders_clean = false;
                if (delptr != deltable.end()) {
                    uint64_t market_id = delptr->id;

                    // delete de oldest history entry
                    history htable(contract, market_id);
                    auto hptr = htable.begin();
                    if (hptr != htable.end()) {
                        report += string("|delmarket-history-prune:")+ std::to_string((unsigned long)hptr->id)  + "," + std::to_string((unsigned long)market_id);
                        PRINT("  => history.erase(): ",std::to_string((unsigned long)hptr->id), "\n");
                        htable.erase(*hptr);

                        historyall hall_table(contract, contract.value);
                        auto hall_index = hall_table.get_index<name("marketkey")>();
                        historyall_table auxiliar;
                        auxiliar.key = hptr->id;
                        auxiliar.market = market_id;
                        PRINT(" -> auxiliar.key: ",std::to_string((unsigned long long)auxiliar.key), "\n");
                        PRINT(" -> auxiliar.market: ",std::to_string((unsigned long long)auxiliar.market), "\n");
                        PRINT(" -> auxiliar.by_marketkey_key(): ",std::to_string((unsigned long long)auxiliar.by_marketkey_key()), "\n");
                        auto hall_ptr = hall_index.find(auxiliar.by_marketkey_key());
                        check(hall_ptr != hall_index.end(), "ERROR, el indice no encara");
                        check(hall_ptr->key == hptr->id, create_error_id2("ERROR, keys are not equal:", hall_ptr->key, hptr->id).c_str());
                        check(hall_ptr->market == market_id, create_error_id2("ERROR, markets are not equal:", hall_ptr->market, market_id).c_str());

                        PRINT("  => historyall.erase(): ",std::to_string((unsigned long)hall_ptr->id), "\n");
                        hall_table.erase(*hall_ptr);
                        exp.amount += 5 * gconf.maint_reward_delmarkets_exp;
                        pts.amount += 5 * gconf.maint_reward_delmarkets_pts;
                    } else {
                        PRINT(" -> history_clean \n");
                        history_clean = true;
                    }

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

                    if (history_clean && sellorders_clean) {
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

            string aux_maintenance_from_history(asset& pts, asset& exp) {
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_history()\n");
                PRINT(" pts: ", pts.to_string(), "\n");
                PRINT(" exp: ", exp.to_string(), "\n");

                state gconf = vapaee::dex::global::get(); 

                string report;
                int hprune_days = vapaee::dex::global::get().hprune;
                int kprune_days = vapaee::dex::global::get().kprune;
                time_point_sec _now = vapaee::dex::global::get_now_time_point_sec();
                time_point_sec _expire_h = time_point_sec(_now.utc_seconds - hprune_days * 24 * 60 * 60);
                time_point_sec _expire_k = time_point_sec(_now.utc_seconds - kprune_days * 24 * 60 * 60);

                PRINT(" -> _now:    ",std::to_string((unsigned long)_now.utc_seconds)," \n");
                PRINT(" -> _expire_h: ",std::to_string((unsigned long)_expire_h.utc_seconds)," \n");
                PRINT(" -> _expire_k: ",std::to_string((unsigned long)_expire_k.utc_seconds)," \n");

                historyall hall_table(contract, contract.value);
                auto hall_ptr = hall_table.begin();
                if (hall_ptr != hall_table.end()) {
                    if (hall_ptr->date < _expire_h) {
                        history h_table(contract, hall_ptr->market);
                        auto h_ptr = h_table.find(hall_ptr->key);
                        check(h_ptr != h_table.end(), create_error_id2(ERROR_AMFH_1, hall_ptr->key, hall_ptr->market));

                        report += string("|history-prune:") + std::to_string((unsigned long)hall_ptr->key) + "," + std::to_string((unsigned long)hall_ptr->market);

                        PRINT(" => history.erase(): ",std::to_string((unsigned long)h_ptr->id)," \n");
                        PRINT(" => historyall.erase(): ",std::to_string((unsigned long)hall_ptr->id)," \n");
                        h_table.erase(*h_ptr);
                        hall_table.erase(*hall_ptr);
                        exp.amount += 4 * gconf.maint_reward_history_exp;
                        pts.amount += 6 * gconf.maint_reward_history_pts;
                    }
                }
                
                historyblock blocks_table(contract, contract.value);
                auto blocks_ptr = blocks_table.begin();
                if (blocks_ptr != blocks_table.end()) {
                    if (blocks_ptr->date < _expire_k) {                        
                        report += string("|historyblock-prune:") + std::to_string((unsigned long)blocks_ptr->id) + "," + std::to_string((unsigned long)blocks_ptr->hour);
                        PRINT(" => historyblock.erase(): ",std::to_string((unsigned long)blocks_ptr->id)," \n");
                        blocks_table.erase(*blocks_ptr);
                        exp.amount += 8 * gconf.maint_reward_history_exp;
                        pts.amount += 10 * gconf.maint_reward_history_pts;                      
                    }
                }

                PRINT("vapaee::dex::maintenance::aux_maintenance_from_history() ...\n");
                return report;
            }

            string aux_maintenance_from_events(asset& pts, asset& exp) {
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_events()\n");
                PRINT(" pts: ", pts.to_string(), "\n");
                PRINT(" exp: ", exp.to_string(), "\n");

                state gconf = vapaee::dex::global::get(); 
                
                string report;
                int eprune_days = vapaee::dex::global::get().eprune;
                time_point_sec _now = vapaee::dex::global::get_now_time_point_sec();
                time_point_sec _expire = time_point_sec(_now.utc_seconds - eprune_days * 24 * 60 * 60);

                PRINT(" -> _now:       ",std::to_string((unsigned long)_now.utc_seconds)," \n");
                PRINT(" -> _expire:    ",std::to_string((unsigned long)_expire.utc_seconds)," \n");

                events etable(contract, contract.value);
                int max = 20;
                int count = 0;
                for (auto eptr = etable.begin(); eptr != etable.end() && count < max; eptr = etable.begin()) {
                    eptr++; //we skip the very first event witch holds the max id for events
                    if (eptr == etable.end()) {
                        break;
                    } else {
                        if (eptr->date < _expire) {
                            count++;
                            PRINT(" => events.erase(): ",std::to_string((unsigned long)eptr->id)," '", eptr->params, "' \n");
                            etable.erase(*eptr);
                        } else {
                            PRINT(" -> not expired: ",std::to_string((unsigned long)eptr->date.utc_seconds)," '", eptr->params, "' \n");
                            break;
                        }
                    }                    
                }
                if (count>0) {
                    report += string("|events-prune:") + std::to_string(count);
                    exp.amount += count * gconf.maint_reward_events_exp;
                    pts.amount += count * gconf.maint_reward_events_pts;
                }

                PRINT("vapaee::dex::maintenance::aux_maintenance_from_events() ...\n");
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
                    exp.amount += count * gconf.maint_reward_events_exp;
                    pts.amount += count * gconf.maint_reward_events_pts;
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
                            exp.amount += 5 * gconf.maint_reward_events_exp;
                            pts.amount += 10 * gconf.maint_reward_events_pts;                            
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
                PRINT("       -- MAINTENANCE: history       --\n");
                report += aux_maintenance_from_history(points, exp);

                PRINT("       --------------------------------\n");
                PRINT("       -- MAINTENANCE: events        --\n");
                report += aux_maintenance_from_events(points, exp);

                PRINT("       --------------------------------\n");
                PRINT("       -- MAINTENANCE: ballots       --\n");
                report += aux_maintenance_from_ballots(points, exp);

                PRINT("       --------------------------------\n");
                PRINT("       -- MAINTENANCE: weekly points --\n");
                report += aux_maintenance_from_points(points, exp);

                if (points.amount > 0) {
                    action(
                        permission_level{contract,name("active")},
                        contract,
                        name("reward"),
                        std::make_tuple(credits_to, points, exp)
                    ).send();
                }

                PRINT("vapaee::dex::maintenance::action_do_maintenance() ------------------------------ (end)\n");
            }
        };     
    };
};
