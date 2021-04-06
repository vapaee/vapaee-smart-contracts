#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/constants.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/exchange.hpp>
#include <vapaee/dex/modules/record.hpp>
#include <vapaee/dex/modules/global.hpp>
#include <vapaee/dex/modules/experience.hpp>

namespace vapaee {
    namespace dex {

        using namespace utils;

        namespace maintenance {

            
            // maintenance ------------------------------------------------------------------
            void aux_cancel_sell_order(name owner, uint64_t id, uint64_t market) {
                PRINT("vapaee::dex::maintenance::aux_cancel_sell_order()\n");
                uint64_t can_market = market;
                if (market % 2 != 0) {
                    can_market--;
                }

                PRINT(" owner:      ", owner.to_string(), "\n");
                PRINT(" can_market: ", std::to_string((unsigned long) can_market), "\n");
                PRINT(" market:     ", std::to_string((unsigned long) market), "\n");

                sellorders selltable(contract, market);
                asset return_amount;
                name table = aux_get_table_from_market(market);
                
                uint64_t order_id = id;
                auto itr = selltable.find(order_id);
                check(itr != selltable.end(), "buy order not found");
                check(itr->owner == owner, "attemp to delete someone elses buy order");
                return_amount = itr->selling;
                uint64_t client = itr->client;
                PRINT("  return_amount: ", return_amount.to_string(), "\n");
                selltable.erase(*itr);

                // ake out the order from the user personal order registry
                userorders buyerorders(contract, owner.value);
                auto buyer_itr = buyerorders.find(market);
                
                check(buyer_itr != buyerorders.end(), "ERROR: cómo que no existe? No fue registrado antes?");
                // take the order out of the buyer personal order registry
                buyerorders.modify(*buyer_itr, same_payer, [&](auto & a){
                    std::vector<uint64_t> newlist;
                    std::copy_if (a.ids.begin(), a.ids.end(), std::back_inserter(newlist), [&](uint64_t i){return i!=order_id;} );
                    a.ids = newlist;
                });
                // if there's no orders left, erase the entire table entry
                buyer_itr = buyerorders.find(market);
                if (buyer_itr != buyerorders.end() && buyer_itr->ids.size() == 0) {
                    buyerorders.erase(*buyer_itr);
                }

                // skip ordersummary
                PRINT("  --> swapdeposit: swap ", return_amount.to_string(), " to ", owner.to_string(), "\n");
                action(
                    permission_level{contract,name("active")},
                    contract,
                    name("swapdeposit"),
                    std::make_tuple(contract, owner, return_amount, string(TEXT_ACSO_2))
                ).send();
                asset _asset;
                vapaee::dex::record::aux_trigger_event(return_amount.symbol.code(), name("cancel"), owner, contract, return_amount, _asset, _asset);

                asset return_amount_real = aux_get_real_asset(return_amount);
                PRINT("  --> withdraw: transfer ", return_amount_real.to_string(), " to ", owner.to_string(), "\n");
                action(
                    permission_level{contract,name("active")},
                    contract,
                    name("withdraw"),
                    std::make_tuple(owner, return_amount_real, client)
                ).send();
                vapaee::dex::record::aux_trigger_event(return_amount_real.symbol.code(), name("withdraw"), owner, contract, return_amount_real, _asset, _asset);

                PRINT("vapaee::dex::maintenance::aux_cancel_sell_order() ...\n");
            }

            string aux_maintenance_from_delmarkets(asset& pts, asset& exp) {
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_delmarkets()\n");
                PRINT(" pts: ", pts.to_string(), "\n");
                PRINT(" exp: ", exp.to_string(), "\n");
                
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
                        exp.amount += 5;
                        pts.amount += 5;
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
                        aux_cancel_sell_order(sptr->owner, sptr->id, market_id);
                        exp.amount += 5;
                        pts.amount += 5;
                    } else {
                        PRINT(" -> sellorders_clean \n");
                        sellorders_clean = true;
                    }

                    if (history_clean && sellorders_clean) {
                        PRINT("  => delmarkets.erase(): ",std::to_string((unsigned long)delptr->id), " DELETE delmarket!!!!\n");
                        report += string("|delmarket-erase-market:") + std::to_string((unsigned long)market_id);
                        deltable.erase(*delptr);
                        exp.amount += 3;
                        pts.amount += 5;
                    }

                }
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_delmarkets() ...\n");                
                return report;
            }

            string aux_maintenance_from_history(asset& pts, asset& exp) {
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_history()\n");
                PRINT(" pts: ", pts.to_string(), "\n");
                PRINT(" exp: ", exp.to_string(), "\n");

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
                        exp.amount += 4;
                        pts.amount += 6;
                    }
                }
                
                historyblock blocks_table(contract, contract.value);
                auto blocks_ptr = blocks_table.begin();
                if (blocks_ptr != blocks_table.end()) {
                    if (blocks_ptr->date < _expire_k) {                        
                        report += string("|historyblock-prune:") + std::to_string((unsigned long)blocks_ptr->id) + "," + std::to_string((unsigned long)blocks_ptr->hour);
                        PRINT(" => historyblock.erase(): ",std::to_string((unsigned long)blocks_ptr->id)," \n");
                        blocks_table.erase(*blocks_ptr);
                        exp.amount += 8;
                        pts.amount += 10;                        
                    }
                }

                PRINT("vapaee::dex::maintenance::aux_maintenance_from_history() ...\n");
                return report;
            }

            string aux_maintenance_from_events(asset& pts, asset& exp) {
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_events()\n");
                PRINT(" pts: ", pts.to_string(), "\n");
                PRINT(" exp: ", exp.to_string(), "\n");
                
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
                    exp.amount += (uint64_t)(((float)count)/2.0);
                    pts.amount += count*2;                    
                }

                PRINT("vapaee::dex::maintenance::aux_maintenance_from_events() ...\n");
                return report;
            }

            string aux_maintenance_from_points(asset& pts, asset& exp) {
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_points()\n");
                PRINT(" pts: ", pts.to_string(), "\n");
                PRINT(" exp: ", exp.to_string(), "\n");

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
                    exp.amount += (uint64_t)(((float)count)/2.0);
                    pts.amount += count*2;                
                }

                PRINT("vapaee::dex::maintenance::aux_maintenance_from_points() ...\n");
                return report;
            }

            string aux_maintenance_from_ballots(asset& pts, asset& exp) {
                PRINT("vapaee::dex::maintenance::aux_maintenance_from_ballots()\n");
                PRINT(" pts: ", pts.to_string(), "\n");
                PRINT(" exp: ", exp.to_string(), "\n");
                
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
                            exp.amount += 5;
                            pts.amount += 10;                            
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

                if (report.size() > 0) {
                    vapaee::dex::record::aux_register_event(contract, name("maintenance"), string("detail") + report);
                }

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
