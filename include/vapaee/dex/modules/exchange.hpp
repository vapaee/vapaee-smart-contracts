#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/constants.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/dex/modules/error.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/record.hpp>
#include <vapaee/dex/modules/market.hpp>
#include <vapaee/dex/modules/client.hpp>
#include <vapaee/dex/modules/deposit.hpp>
#include <vapaee/dex/modules/experience.hpp>

namespace vapaee {
    namespace dex {

        using namespace error;
        using namespace utils;
        using namespace client;
        using namespace record;
        using namespace market;
        using namespace deposit;

        namespace exchange {
         
            /*bool aux_is_market_being_deleted(uint64_t market_id) {
                PRINT("vapaee::dex::exchange::aux_is_market_being_deleted()\n");
                PRINT(" can_market: ", std::to_string((unsigned long) market_id), "\n");
                delmarkets table(contract, contract.value);
                auto ptr = table.find(market_id);
                bool found = ptr != table.end();
                PRINT("vapaee::dex::exchange::aux_is_market_being_deleted() ...\n");
                return found;
            }*/

            void aux_cancel_sell_order(name owner, uint64_t can_market, uint64_t market, const std::vector<uint64_t> & orders) {
                // viterbotelos, acorn.telosd, acorn.telosd, [1]
                PRINT("vapaee::dex::exchange::aux_cancel_sell_order()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" can_market: ", std::to_string((unsigned long) can_market), "\n");
                PRINT(" market: ", std::to_string((unsigned long) market), "\n");
                PRINT(" orders.size(): ", orders.size(), "\n");

                sellorders selltable(contract, market);
                asset return_amount;
                name table = aux_get_table_from_market(market);
                
                ordersummary o_summary(contract, contract.value);
                auto orders_ptr = o_summary.find(can_market);
                bool reverse_scope = can_market != market;

                // Register event
                aux_register_event(owner, name("cancel.order"), table.to_string() + "|" + std::to_string(orders.size()));

                for (int i=0; i<orders.size(); i++) {
                    uint64_t order_id = orders[i];
                    auto itr = selltable.find(order_id);
                    check(itr != selltable.end(), "buy order not found");
                    check(itr->owner == owner, "attemp to delete someone elses buy order");
                    return_amount = itr->selling;
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

                    //if (!aux_is_market_being_deleted(can_market)) {
                    // take out the registry for this canceled order
                    check(orders_ptr != o_summary.end(), create_error_id1(ERROR_ACSO_1, can_market).c_str());
                    if (!reverse_scope) {
                        // we are canceling a sell-order so we decrement the supply
                        o_summary.modify(*orders_ptr, same_payer, [&](auto & a){
                            a.supply.orders--;
                            check(a.supply.total.symbol == return_amount.symbol,
                                create_error_asset2(ERROR_ACSO_2, a.supply.total, return_amount).c_str());
                            a.supply.total -= return_amount;
                        });
                    } else {
                        // we are consuming a sell-order so we decrement the demand
                        o_summary.modify(*orders_ptr, same_payer, [&](auto & a){
                            a.demand.orders--;
                            check(a.demand.total.symbol == return_amount.symbol,
                                create_error_asset2(ERROR_ACSO_2, a.demand.total, return_amount).c_str());
                            a.demand.total -= return_amount;
                        });
                    }
                    //}
                    asset _asset;

                    PRINT("  --> swapdeposit: ", return_amount.to_string(), " to ", owner.to_string(), "\n");
                    action(
                        permission_level{contract,name("active")},
                        contract,
                        name("swapdeposit"),
                        std::make_tuple(contract, owner, return_amount, string(TEXT_ACSO_1))
                    ).send();
                    aux_trigger_event(return_amount.symbol.code(), name("cancel"), owner, contract, return_amount, _asset, _asset);
                    
                    // auto-withraw
                    asset return_real_amount = aux_get_real_asset(return_amount);
                    PRINT("  --> withdraw: transfer ", return_real_amount.to_string(), " to ", owner.to_string(), "\n");
                    action(
                        permission_level{contract,name("active")},
                        contract,
                        name("withdraw"),
                        std::make_tuple(owner, return_real_amount, itr->client)
                    ).send();
                    aux_trigger_event(return_real_amount.symbol.code(), name("withdraw"), owner, contract, return_real_amount, _asset, _asset);
                    
                    // we do some maintenance
                    action(
                        permission_level{contract,name("active")},
                        contract,
                        name("maintenance"),
                        std::make_tuple(owner)
                    ).send();
                }

                userorders buyerorders(contract, owner.value);
                auto buyer_itr = buyerorders.find(market);
                if (buyer_itr != buyerorders.end() && buyer_itr->ids.size() == 0) {
                    buyerorders.erase(*buyer_itr);
                }
            }

            void action_cancel(name owner, name type, const symbol_code & token_a, const symbol_code & token_p, const std::vector<uint64_t> & orders) {
                // viterbotelos, sell, ACORN, TELOSD, [1]
                PRINT("vapaee::dex::exchange::action_cancel()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" type: ", type.to_string(), "\n");
                PRINT(" token_a: ",  token_a.to_string(), "\n");
                PRINT(" token_p: ",  token_p.to_string(), "\n");
                PRINT(" orders.size(): ", std::to_string((int) orders.size()), "\n");

                require_auth(owner);

                // create scope for the orders table
                uint64_t buy_market = aux_get_market_id(token_a, token_p);
                uint64_t sell_market = aux_get_market_id(token_p, token_a);
                uint64_t can_market = aux_get_canonical_market(token_a, token_p);

                if (type == name("sell")) {
                    aux_cancel_sell_order(owner, can_market, buy_market, orders);
                }

                if (type == name("buy")) {
                    aux_cancel_sell_order(owner, can_market, sell_market, orders);
                }

                PRINT("vapaee::dex::exchange::action_cancel() ...\n");
            }

            // ----------------------------------------------------------
            void aux_clone_user_deposits(name owner, vector<asset> & depos) {
                PRINT("vapaee::dex::exchange::aux_clone_user_deposits()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                
                deposits depositstable(contract, owner.value);
                for (auto itr = depositstable.begin(); itr != depositstable.end(); itr++) {
                    PRINT(" - deposit: ", itr->amount.to_string(), "\n");
                    depos.push_back(itr->amount);            
                }

                PRINT(" deposits.size(): ", depos.size(), "\n");
                PRINT("vapaee::dex::exchange::aux_clone_user_deposits() ...\n");
            }

            asset aux_apply_maker_fees(const asset& money) {
                PRINT("vapaee::dex::exchange::aux_apply_maker_fees()\n");
                PRINT("        money: ", money.to_string(), "\n");
                asset result = money;
                asset maker_fee = vapaee::dex::global::get().maker_fee;
                PRINT(" >  maker_fee: ", maker_fee.to_string(), "\n");
                
                uint64_t unit = pow(10.0, maker_fee.symbol.precision());
                float maker_fee_percentage = (float) ((float)maker_fee.amount / (float)unit);
                result.amount = (uint64_t) (money.amount * maker_fee_percentage);
                PRINT(" ->     float: ", std::to_string(maker_fee_percentage), "\n");
                PRINT(" ->    result: ", result.to_string(), "\n");

                PRINT("vapaee::dex::exchange::aux_apply_maker_fees()\n");
                return result;
            }

            asset aux_apply_taker_fees(const asset& money) {
                PRINT("vapaee::dex::exchange::aux_apply_taker_fees()\n");
                PRINT("        money: ", money.to_string(), "\n");
                asset result = money;
                asset taker_fee = vapaee::dex::global::get().taker_fee;
                PRINT(" >  taker_fee: ", taker_fee.to_string(), "\n");
                
                uint64_t unit = pow(10.0, taker_fee.symbol.precision());
                float taker_fee_percentage = (float) ((float)taker_fee.amount / (float)unit);
                result.amount = (uint64_t) (money.amount * taker_fee_percentage);
                PRINT(" ->     float: ", std::to_string(taker_fee_percentage), "\n");
                PRINT(" ->    result: ", result.to_string(), "\n");

                PRINT("vapaee::dex::exchange::aux_apply_taker_fees()\n");
                return result;
            }
                        
            void aux_reward_users(name maker, name taker, const asset &tlos_volume) {
                PRINT("vapaee::dex::exchange::aux_reward_users()\n");
                PRINT(" maker: ", maker.to_string(), "\n");
                PRINT(" taker: ", taker.to_string(), "\n");
                PRINT(" tlos_volume: ", tlos_volume.to_string(), "\n");

                double unit = (double)pow(10.0, tlos_volume.symbol.precision());
                double realamount = ( (double)tlos_volume.amount / unit );
                
                PRINT(" -> realamount: ", std::to_string(realamount), "\n");
                if (tlos_volume.amount > 0) {
                    uint64_t mpts = (uint64_t) (realamount * 1.0);
                    uint64_t mexp = (uint64_t) (realamount * 0.5);
                    uint64_t tpts = (uint64_t) (realamount * 2.0);
                    uint64_t texp = (uint64_t) (realamount * 1.0);

                    PRINT(" -> mpts: ", std::to_string((unsigned long)mpts), "\n");
                    PRINT(" -> mexp: ", std::to_string((unsigned long)mexp), "\n");
                    PRINT(" -> tpts: ", std::to_string((unsigned long)tpts), "\n");
                    PRINT(" -> texp: ", std::to_string((unsigned long)texp), "\n");

                    asset maker_points = asset(mpts, vapaee::dex::experience::POINTS_SYMBOL);
                    asset maker_exp    = asset(mexp, vapaee::dex::experience::EXP_SYMBOL);
                    asset taker_points = asset(tpts, vapaee::dex::experience::POINTS_SYMBOL);
                    asset taker_exp    = asset(texp, vapaee::dex::experience::EXP_SYMBOL);

                    PRINT(" -> maker_points: ", maker_points.to_string(), "\n");
                    PRINT(" -> maker_exp:    ", maker_exp.to_string(), "\n");
                    PRINT(" -> taker_points: ", taker_points.to_string(), "\n");
                    PRINT(" -> taker_exp:    ", taker_exp.to_string(), "\n");

                    // reward taker 
                    action(
                        permission_level{contract,name("active")},
                        contract,
                        name("reward"),
                        std::make_tuple(taker, taker_points, taker_exp)
                    ).send();

                    // reward maker
                    action(
                        permission_level{contract,name("active")},
                        contract,
                        name("reward"),
                        std::make_tuple(maker, maker_points, maker_exp)
                    ).send();
                }
                PRINT("vapaee::dex::exchange::aux_reward_users()\n");
            }

            void aux_generate_sell_order(bool inverted, name owner, uint64_t market_buy, uint64_t market_sell, asset total, asset payment, asset price, asset inverse, name ram_payer, uint64_t sell_client) {
                PRINT("vapaee::dex::exchange::aux_generate_sell_order()\n");
                PRINT(" inverted: ", std::to_string(inverted), "\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" market_buy: ", std::to_string((long unsigned) market_buy), "\n");
                PRINT(" market_sell: ", std::to_string((long unsigned) market_sell), "\n");
                PRINT(" total: ", total.to_string(), "\n");        // total: 1.00000000 TLOS
                PRINT(" payment: ", payment.to_string(), "\n");    // payment: 2.50000000 CNT
                PRINT(" price: ", price.to_string(), "\n");        // price: 2.50000000 CNT
                PRINT(" inverse: ", inverse.to_string(), "\n");    // inverse: 0.40000000 TLOS
                PRINT(" ram_payer: ", ram_payer.to_string(), "\n");
                PRINT(" sell_client: ", std::to_string((long unsigned) sell_client), "\n");
                
                sellorders buytable(contract,  market_buy);
                sellorders selltable(contract, market_sell);

                ordersummary o_summary(contract, contract.value);
                symbol_code  A = total.symbol.code();
                symbol_code  B = payment.symbol.code();
                asset _asset; // aux var;
                
                uint64_t can_market = aux_get_canonical_market(A, B);
                uint64_t inv_market = aux_get_inverted_market(A, B);
                uint64_t market = aux_get_market_id(A, B);
                auto orders_ptr = o_summary.find(can_market);
                bool reverse_scope = market != can_market;

                // sellorders selltable(contract, scope.value);
                auto buy_index = buytable.get_index<name("price")>();
                // auto sell_index = selltable.get_index<"price"_n>();
                auto sell_index = selltable.get_index<name("price")>();
                asset remaining = total;
                asset remaining_payment = payment;
                asset current_price;
                asset current_inverse;
                asset current_total;
                asset current_payment = payment;
                name maker, taker;
                uint64_t maker_client, taker_client;
                // asset inverse = vapaee::utils::inverse(price, total.symbol);
                sell_order_table order;
                
                vector<asset> deposits;
                aux_put_deposits_on_user_ram(owner, payment);
                aux_clone_user_deposits(owner, deposits);

                tokens tokenstable(contract, contract.value);
                auto atk_itr = tokenstable.find(total.symbol.code().raw());
                auto ptk_itr = tokenstable.find(price.symbol.code().raw());
                check(atk_itr != tokenstable.end(), (string("Token ") + total.symbol.code().to_string() + " not registered").c_str());
                check(ptk_itr != tokenstable.end(), (string("Token ") + price.symbol.code().to_string() + " not registered").c_str());
                // check(atk_itr->precision == total.symbol.precision(), aux_error_1(total, atk_itr->precision).c_str());
                // check(ptk_itr->precision == price.symbol.precision(), aux_error_1(price, ptk_itr->precision).c_str());
                check(price.symbol != total.symbol, (string("price token symbol ") + price.symbol.code().to_string() + " MUST be different from total").c_str());
    
                uint64_t total_unit = pow(10.0, total.symbol.precision());
                uint64_t price_unit = pow(10.0, price.symbol.precision());
                int deals = 0;
                // iterate over a list or buy order from the maximun price down
                for (auto b_ptr = buy_index.begin(); b_ptr != buy_index.end(); b_ptr = buy_index.begin()) {
                    check(b_ptr->price.symbol == inverse.symbol,
                        create_error_asset2(ERROR_AGSO_1, b_ptr->price, inverse).c_str());
                    PRINT(" compare: (price<=inverse) ??  - (", b_ptr->price.to_string(), " <= ", inverse.to_string(), ") ??? \n");
                    if (b_ptr->price.amount <= inverse.amount) {
                        deals++;
                        // ------ DEAL !!!------
                        current_price = b_ptr->price;   // TLOS
                        PRINT("------ DEAL!!------ price: ", current_price.to_string(),"\n");
                        maker = b_ptr->owner;
                        maker_client = b_ptr->client;
                        taker = owner;
                        taker_client = sell_client;
                        PRINT("              maker: ", maker.to_string() ,"\n");
                        PRINT("      current_price: ", current_price.to_string() ,"\n");
                        PRINT("       b_ptr->total: ", b_ptr->total.to_string(), " > remaining: ", remaining.to_string()," ?\n");
    
                        check(b_ptr->total.symbol == remaining.symbol,
                            create_error_asset2(ERROR_AGSO_2, b_ptr->total, remaining).c_str());

                        if (b_ptr->total > remaining) { // CNT
                            // maker wants more that the user is selling -> reduces maker order amount
                            current_total = remaining;  // CNT                          
                            current_payment.amount = utils::multiply(remaining, b_ptr->inverse);

                            // // this code is useful to hot-debugging
                            // check(owner.value != name("viterbotelos").value,
                            //     create_error_asset4("DEBUG IN PROGRESS. PLEASE WAIT",
                            //     current_payment, b_ptr->inverse, remaining, b_ptr->total).c_str());       

                            buytable.modify(*b_ptr, aux_get_modify_payer(ram_payer), [&](auto& a){
                                // double percent = (double)remaining.amount / (double)a.total.amount;
                                check(a.total.symbol == remaining.symbol,
                                    create_error_asset2(ERROR_AGSO_3, a.total, remaining).c_str());
                                check(a.selling.symbol == current_payment.symbol,
                                    create_error_asset2(ERROR_AGSO_4, a.selling, current_payment).c_str());
                                a.total   -= remaining;          // CNT
                                a.selling -= current_payment;    // TLOS
                            });

                            PRINT("    payment (1): ", current_payment.to_string(),"\n");

                            // decrese the total in registry for this incompleted order
                            check(orders_ptr != o_summary.end(), "table MUST exist but it does not");
                            o_summary.modify(*orders_ptr, ram_payer, [&](auto & a){
                                PRINT("        a.total:  ", a.demand.total.to_string(),"\n");
                                PRINT("        payment:  ", current_payment.to_string(),"\n");
                                
                                if (!reverse_scope) {
                                    // we are consuming part of a buy-order so we decrement the demand
                                    check(a.demand.total.symbol == current_payment.symbol,
                                        create_error_asset2(ERROR_AGSO_5, a.demand.total, current_payment).c_str());             
                                    a.demand.total -= current_payment;
                                } else {
                                    // we are consuming part of a sell-order so we decrement the supply
                                    check(a.supply.total.symbol == current_payment.symbol,
                                        create_error_asset2(ERROR_AGSO_5, a.supply.total, current_payment).c_str());
                                    a.supply.total -= current_payment;
                                }
                            });

                        } else {
                            // maker gets all amount wanted -> destroy order                        
                            uint64_t buy_id = b_ptr->id;
                            current_total = b_ptr->total;
                            current_payment = b_ptr->selling;
                            buytable.erase(*b_ptr);
                            PRINT("    payment (2): ", current_payment.to_string(),"\n");

                            // register order in user personal order registry
                            userorders makerorders(contract, maker.value);     
                            auto maker_itr = makerorders.find(market_buy);
                            check(maker_itr != makerorders.end(), "ERROR: cómo que no existe? No fue registrado antes? maker? market_buy?");
                            check(orders_ptr != o_summary.end(), "table MUST exist but it does not");

                            // take the order out of the maker personal order registry
                            makerorders.modify(*maker_itr, ram_payer, [&](auto & a){
                                std::vector<uint64_t> newlist;
                                std::copy_if (a.ids.begin(), a.ids.end(), std::back_inserter(newlist), [&](uint64_t i){return i!=buy_id;} );
                                a.ids = newlist;
                            });
                            // if there's no orders left, erase the entire table entry
                            maker_itr = makerorders.find(market_buy);
                            if (maker_itr != makerorders.end() && maker_itr->ids.size() == 0) {
                                makerorders.erase(*maker_itr);
                            }

                            if (!reverse_scope) {
                                // we are consuming a buy-order so we decrement the demand
                                o_summary.modify(*orders_ptr, ram_payer, [&](auto & a){
                                    a.demand.orders--;
                                    check(a.demand.total.symbol == current_payment.symbol,
                                        create_error_asset2(ERROR_AGSO_6, a.demand.total, current_payment).c_str());
                                    a.demand.total -= current_payment;
                                });
                            } else {
                                // we are consuming a sell-order so we decrement the supply
                                o_summary.modify(*orders_ptr, ram_payer, [&](auto & a){
                                    a.supply.orders--;
                                    check(a.supply.total.symbol == current_payment.symbol,
                                        create_error_asset2(ERROR_AGSO_6, a.supply.total, current_payment).c_str());
                                    a.supply.total -= current_payment;
                                });
                            }

                        }

                        check(remaining.symbol == current_total.symbol,
                            create_error_asset2(ERROR_AGSO_7, remaining, current_total).c_str());
                        check(remaining_payment.symbol == current_payment.symbol,
                            create_error_asset2(ERROR_AGSO_8, remaining_payment, current_payment).c_str());

                        remaining -= current_total;
                        remaining_payment -= current_payment;
                        asset taker_fee, maker_fee, taker_gains, maker_gains;

                        // calculate fees
                        maker_fee = aux_apply_maker_fees(current_total);
                        maker_gains = current_total - maker_fee;
                        // taker gets total minus maker_fee

                        taker_fee = aux_apply_taker_fees(current_payment);
                        taker_gains = current_payment - taker_fee;

                        // transfer CNT to maker 
                        action(
                            permission_level{contract,name("active")},
                            contract,
                            name("swapdeposit"),
                            std::make_tuple(taker, maker, maker_gains, string("exchange made for ") + current_payment.to_string())
                        ).send();
                        PRINT("     -- transfer ", maker_gains.to_string(), " to ", maker.to_string()," (maker)\n");


                        // transfer to contract fees on CNT
                        // at this moment maker_fee is still in the owner's deposits. So it must be swaped to the contract before earning it
                        if (maker_fee.amount > 0) {
                            action(
                                permission_level{contract,name("active")},
                                contract,
                                name("swapdeposit"),
                                std::make_tuple(taker, contract, maker_fee, string("exchange made for ") + current_total.to_string())
                            ).send();
                            PRINT("     -- charge fees ", maker_fee.to_string(), " to ", maker.to_string()," (maker)\n");
                        }
                            
                        // transfer TLOS to taker (TLOS the belongs to maker but the contracxts holds them)
                        action(
                            permission_level{contract,name("active")},
                            contract,
                            name("swapdeposit"),
                            std::make_tuple(contract, taker, taker_gains, string("exchange made for ") + current_total.to_string())
                        ).send();
                        PRINT("     -- transfer ", taker_gains.to_string(), " to ", taker.to_string()," (taker)\n");

                        // convert deposits to earnings
                        // Now the contract's deposits includes the maker_fee, so it can be transformed to ernings
                        if (taker_fee.amount > 0) {
                            action(
                                permission_level{contract,name("active")},
                                contract,
                                name("deps2earn"),
                                std::make_tuple(taker_client, taker_fee)
                            ).send();
                            PRINT("     -- sending fees ", taker_fee.to_string(), " to ", std::to_string((unsigned long)taker_client)," (taker_client)\n");
                        }

                        // The taker_fee were already included in the contract's deposits, so no swap was needed.
                        // It can be earned directly
                        if (maker_fee.amount > 0) {
                            action(
                                permission_level{contract,name("active")},
                                contract,
                                name("deps2earn"),
                                std::make_tuple(maker_client, maker_fee)
                            ).send();
                            PRINT("     -- sending fees ", maker_fee.to_string(), " to ", std::to_string((unsigned long)maker_client)," (maker_client)\n");
                        }
                        // saving the transaction in history
                        current_inverse = utils::inverse(current_price, current_payment.symbol);
                        // PRINT("   - current_payment: ", current_payment.to_string(), "\n");  // 1.00000000 EDNA
                        // PRINT("   - inverse:         ", inverse.to_string(), "\n");
                        // PRINT("   - current_price:   ", current_price.to_string(), "\n");    // 0.00047800 TLOS
                        // PRINT("   - current_inverse: ", current_inverse.to_string(), "\n");
                        aux_register_transaction_in_history(inverted, maker, taker, current_inverse, current_price, current_payment, current_total, maker_fee, taker_fee);
                        
                        // auto-withraw -----------
                        asset maker_gains_real = aux_get_real_asset(maker_gains);
                        PRINT("  --> withdraw: transfer ", maker_gains_real.to_string(), " to ", maker.to_string(), "\n");
                        action(
                            permission_level{contract,name("active")},
                            contract,
                            name("withdraw"),
                            std::make_tuple(maker, maker_gains_real,  maker_client)
                        ).send();
                        aux_trigger_event(maker_gains_real.symbol.code(), name("withdraw"), maker, contract, maker_gains_real, _asset, _asset);
                        
                        asset taker_gains_real = aux_get_real_asset(taker_gains);
                        PRINT("  --> withdraw: transfer ", taker_gains_real.to_string(), " to ", maker.to_string(), "\n");
                        action(
                            permission_level{contract,name("active")},
                            contract,
                            name("withdraw"),
                            std::make_tuple(taker, taker_gains_real, taker_client)
                        ).send();
                        aux_trigger_event(taker_gains_real.symbol.code(), name("withdraw"), taker, contract, taker_gains_real, _asset, _asset);    


                        // experience ------
                        asset tlos_volume;
                        if (current_total.symbol.code() == vapaee::utils::SYS_TKN_CODE) {
                            tlos_volume = current_total;
                        } else if (current_payment.symbol.code() == vapaee::utils::SYS_TKN_CODE) {
                            tlos_volume = current_payment;
                        }

                        aux_reward_users(maker, taker, tlos_volume);

                    } else {
                        break;
                    }

                    PRINT("  remaining:", remaining.to_string(),"\n");
                    if (remaining.amount <= 0) break;
                }

                if (remaining.amount > 0 && remaining_payment.amount > 0) {
                    PRINT("-- final remaining: ", remaining.to_string(), " --\n");
                    // insert sell order
                    
                    payment.amount = remaining_payment.amount;

                    // transfer payment deposits to contract
                    action(
                        permission_level{contract,name("active")},
                        contract,
                        name("swapdeposit"),
                        std::make_tuple(owner, contract, remaining, string(TEXT_AGSO_1))
                    ).send();

                    aux_trigger_event(remaining.symbol.code(), name("order"), owner, contract, remaining, payment, price);

                    PRINT("   remaining: ", remaining.to_string(), "\n");
                    PRINT("   payment: ", payment.to_string(), "\n");
                    PRINT("   price: ", price.to_string(), "\n");
                    PRINT("   inverse: ", inverse.to_string(), "\n");

                    // create order entry
                    // inverse = vapaee::utils::inverse(price, remaining.symbol);
                    uint64_t id = selltable.available_primary_key();
                    PRINT("   selltable.emplace() id:", std::to_string((unsigned long) id),"\n"); 
                    selltable.emplace( ram_payer, [&]( auto& a ) {
                        a.id = id;
                        a.owner = owner;
                        a.price = price;
                        a.inverse = inverse;
                        a.total = payment;
                        a.selling = remaining;
                        a.client = sell_client;
                    });

                    // register new order in the orders table
                    if (orders_ptr == o_summary.end()) {
                        o_summary.emplace( ram_payer, [&]( auto& a ) {
                            if (!reverse_scope) {
                                PRINT("   o_summary.emplace() can_market:", std::to_string((unsigned long)can_market),"\n"); 
                                a.market = can_market;
                                a.sell = remaining.symbol.code();
                                a.pay = payment.symbol.code();            
                                a.supply.orders = 1;
                                a.supply.total = remaining;
                                a.demand.orders = 0;
                                a.demand.total = asset(0, payment.symbol);
                            } else {
                                PRINT("   o_summary.emplace() can_market:", std::to_string((unsigned long)can_market)," INVERSED\n"); 
                                a.market = can_market;
                                a.sell = payment.symbol.code();
                                a.pay = remaining.symbol.code();
                                a.supply.orders = 0;
                                a.supply.total = asset(0, payment.symbol);
                                a.demand.orders = 1;
                                a.demand.total = remaining;
                            }                       
                        });
                    } else {
                        o_summary.modify(*orders_ptr, ram_payer, [&](auto & a){
                            if (!reverse_scope) {
                                a.supply.orders++;
                                a.supply.total += remaining;
                            } else {
                                a.demand.orders++;
                                a.demand.total += remaining;
                            }
                        });
                    }

                    // register order in user personal order registry
                    userorders userorders_table(contract, owner.value);
                    auto user_itr = userorders_table.find(market_sell);
                    if (user_itr == userorders_table.end()) {
                        PRINT("   userorders_table.emplace id:", std::to_string((unsigned long)market_sell),"\n"); 
                        userorders_table.emplace( ram_payer, [&]( auto& a ) {
                            a.table = aux_get_table_from_market(market_sell).to_string();
                            a.market = market_sell;
                            a.ids.push_back(id);
                        });
                    } else {
                        userorders_table.modify(*user_itr, ram_payer, [&](auto & a){
                            a.ids.push_back(id);
                        });
                    }

                    PRINT("  sellorders.emplace(): ", std::to_string((unsigned long long) id), "\n");


                    // we do some maintenance
                    if (deals == 0) {
                        action(
                            permission_level{contract,name("active")},
                            contract,
                            name("maintenance"),
                            std::make_tuple(owner)
                        ).send();

                        // make the user pay for his/her experience RAM storage
                        vapaee::dex::experience::aux_make_user_rampayer(owner);                        
                    }
                }
                
                PRINT("vapaee::dex::exchange::aux_generate_sell_order() ...\n");
            }

            void aux_generate_order(name owner, name type, asset total, asset price, name ram_payer, uint64_t client) {
                PRINT("vapaee::dex::exchange::aux_generate_order()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" type: ", type.to_string(), "\n");
                PRINT(" total: ", total.to_string(), "\n");
                PRINT(" price: ", price.to_string(), "\n");
                PRINT(" client: ", std::to_string((long unsigned) client), "\n");
                
                require_auth(owner);

                // Check if blacklisted
                check(!vapaee::dex::dao::aux_is_token_blacklisted(total.symbol.code()), 
                    create_error_symcode1(ERROR_AGO_1, total.symbol.code()).c_str());
                check(!vapaee::dex::dao::aux_is_token_blacklisted(price.symbol.code()), 
                    create_error_symcode1(ERROR_AGO_2, price.symbol.code()).c_str());

                // create scope for the orders table
                uint64_t market_buy = aux_get_market_id(total.symbol.code(), price.symbol.code());
                uint64_t market_sell = aux_get_market_id(price.symbol.code(), total.symbol.code());

                PRINT(" market_buy: ", std::to_string((long unsigned) market_buy), "\n");
                PRINT(" market_sell: ", std::to_string((long unsigned) market_sell), "\n");
                
                asset inverse = vapaee::dex::utils::inverse(price, total.symbol);
                asset payment = vapaee::dex::utils::payment(total, price);
                
                PRINT(" -> inverse: ", inverse.to_string(), "\n");
                PRINT(" -> payment: ", payment.to_string(), "\n");

                aux_register_event(owner, name(type.to_string() + ".order"), total.to_string() + "|" + price.to_string() );

                // Check client is valid and registered
                vapaee::dex::client::aux_assert_client_is_valid(client);


                if (type == name("sell")) {
                    aux_generate_sell_order(false, owner, market_sell, market_buy, total, payment, price, inverse, ram_payer, client);
                } else if (type == name("buy")) {
                    aux_generate_sell_order(true, owner, market_buy, market_sell, payment, total, inverse, price, ram_payer, client);
                } else {
                    check(false, (string("type must be 'sell' or 'buy' in lower case, got: ") + type.to_string()).c_str());
                }
                
                PRINT("vapaee::dex::exchange::aux_generate_order() ...\n");
            }

            void action_order(name owner, name type, const asset & total, const asset & price, uint64_t client) {
                PRINT("vapaee::dex::exchange::action_order()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" type: ", type.to_string(), "\n");      
                PRINT(" total: ", total.to_string(), "\n");      
                PRINT(" price: ", price.to_string(), "\n");      
                PRINT(" client: ", std::to_string((long unsigned) client), "\n");
                require_auth(owner);

                aux_generate_order(owner, type, total, price, owner, client);

                PRINT("vapaee::dex::exchange::action_order() ...\n");      
            }

            void action_hotfix(int num, name account, asset quantity) {
                
            }
            
        };     
    };
};
