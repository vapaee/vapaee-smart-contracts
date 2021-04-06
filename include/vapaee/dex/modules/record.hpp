#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/dex/modules/global.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/market.hpp>

namespace vapaee {
    namespace dex {
        using namespace utils;
        using namespace market;
        using namespace global;
        
        namespace record {


            // Event ----------
            void aux_trigger_event(const symbol_code & sym_code, name event, name user, name peer, const asset & quantity, const asset & payment, const asset & price) {
                PRINT("vapaee::dex::record::aux_trigger_event()\n");
                PRINT(" sym_code: ",  sym_code.to_string(), "\n");
                PRINT(" event: ",  event.to_string(), "\n");
                PRINT(" user: ",  user.to_string(), "\n");
                PRINT(" peer: ",  peer.to_string(), "\n");
                PRINT(" quantity: ",  quantity.to_string(), "\n");
                PRINT(" payment: ",  payment.to_string(), "\n");
                PRINT(" price: ",  price.to_string(), "\n");

                tokenevents tokeneventstable(contract, sym_code.raw());
                auto itr = tokeneventstable.find(event.value);
                if (itr != tokeneventstable.end()) {
                    name receptor = itr->receptor;
                    PRINT(" -> receptor: ",  receptor.to_string(), "\n");
                    PRINT(" -> carbon copy sent to:: ",  receptor.to_string(), "\n");
                    require_recipient (receptor);

                    action(
                        permission_level{contract,name("active")},
                        receptor,
                        name("dexevent"),
                        std::make_tuple(
                            event,
                            user,
                            peer,
                            quantity,
                            payment,
                            price)
                    ).send();
                }

                PRINT("vapaee::dex::record::aux_trigger_event()... \n");
            }

            // ----------------              

            void aux_register_event(name user, name event, string params) {
                PRINT("vapaee::dex::record::aux_register_event()\n");
                PRINT(" user: ", user.to_string(), "\n");
                PRINT(" event: ", event.to_string(), "\n");
                PRINT(" params: ", params.c_str(), "\n");
                time_point_sec date = get_now_time_point_sec();
                
                events table(contract, contract.value);
                auto header = table.begin();
                if (header == table.end()) {
                    table.emplace(contract, [&](auto & a){
                        a.id = table.available_primary_key();
                        a.user = contract;
                        a.event = name("init");
                        a.params = string("1 event");
                        a.date = date;
                    });
                } else {
                    table.modify(*header, contract, [&](auto & a){
                        a.params = std::to_string((unsigned long) table.available_primary_key()) + string(" events");
                        a.date = date;
                    });
                }

                table.emplace(contract, [&](auto & a){
                    a.id = table.available_primary_key();
                    a.user = user;
                    a.event = event;
                    a.params = params;
                    a.date = date;
                });

                PRINT("vapaee::dex::record::aux_register_event() ...\n");
            }

            name aux_create_label_for_hour (int hh) {
                switch(hh) {
                    case  0: return name("h.zero");
                    case  1: return name("h.one");
                    case  2: return name("h.two");
                    case  3: return name("h.three");
                    case  4: return name("h.four");
                    case  5: return name("h.five");
                    case  6: return name("h.six");
                    case  7: return name("h.seven");
                    case  8: return name("h.eight");
                    case  9: return name("h.nine");
                    case 10: return name("h.ten");
                    case 11: return name("h.eleven");
                    case 12: return name("h.twelve");
                    case 13: return name("h.thirteen");
                    case 14: return name("h.fourteen");
                    case 15: return name("h.fifteen");
                    case 16: return name("h.sixteen");
                    case 17: return name("h.seventeen");
                    case 18: return name("h.eighteen");
                    case 19: return name("h.nineteen");
                    case 20: return name("h.twenty");
                    case 21: return name("h.twentyone");
                    case 22: return name("h.twentytwo");
                    case 23: return name("h.twentythree");
                }
                PRINT("    aux_create_label_for_hour(hh): ERROR:", std::to_string(hh), "\n");
                check(false, "ERROR: bad hour: ");
                return name("error");
            }


            void aux_register_transaction_in_history(bool inverted, name buyer, name seller, asset price, asset inverse, asset payment, asset amount, asset buyfee, asset sellfee) {
                PRINT("vapaee::dex::record::aux_register_transaction_in_history()\n");
                PRINT(" inverted: ", std::to_string(inverted), "\n");
                PRINT(" buyer: ", buyer.to_string(), "\n");
                PRINT(" seller: ", seller.to_string(), "\n");
                PRINT(" amount: ", amount.to_string(), "\n");   // 0.00047800 TLOS
                PRINT(" inverse: ", inverse.to_string(), "\n"); // 0.00047800 TLOS
                PRINT(" price: ", price.to_string(), "\n");     // 2092.05020920 EDNA
                PRINT(" payment: ", payment.to_string(), "\n"); // 1.00000000 EDNA
                PRINT(" buyfee: ", buyfee.to_string(), "\n");   // 0.00000047 TLOS
                PRINT(" sellfee: ", sellfee.to_string(), "\n"); // 0.00200000 EDNA
        
                time_point_sec date = get_now_time_point_sec();
                name tmp_name;
                asset tmp_asset;
                asset tmp_pay;
                name owner = seller;

                symbol_code currency = price.symbol.code();
                if (inverted) {
                    currency = inverse.symbol.code();
                }
                
                symbol_code A = amount.symbol.code();
                symbol_code B = payment.symbol.code();
                name scope = aux_get_canonical_scope_for_symbols(A, B);

                
                bool is_buy = false;
                PRINT(" -> scope: ", scope.to_string(), "\n");

                if (scope == aux_get_scope_for_tokens(B, A)) {
                    // swap buyer / seller names
                    tmp_name = buyer;
                    buyer = seller;
                    seller = tmp_name;

                    // swap fees
                    tmp_asset = buyfee;
                    buyfee = sellfee;
                    buyfee = tmp_asset;

                    // swap amount / payment
                    tmp_asset = amount;
                    amount = payment;
                    payment = tmp_asset;
                    
                    // swap price / inverse
                    tmp_pay = price;
                    price = inverse;
                    inverse = tmp_pay;

                    // swap to "sell" type of transaction
                    is_buy = true;

                    inverted = !inverted;
                    // PRINT(" -> buyer: ", buyer.to_string(), "\n");
                    // PRINT(" -> seller: ", seller.to_string(), "\n");
                    // PRINT(" -> amount: ", amount.to_string(), "\n");
                    // PRINT(" -> price: ", price.to_string(), "\n");
                    // PRINT(" -> buyfee: ", buyfee.to_string(), "\n");
                    // PRINT(" -> sellfee: ", sellfee.to_string(), "\n");
                    // PRINT(" -> payment: ", payment.to_string(), "\n");
                }

                uint64_t market = aux_get_market_id(A, B);
                uint64_t can_market = aux_get_canonical_market(A, B);
                // register event on history table
                history table(contract, can_market);
                uint64_t h_id = table.available_primary_key();
                table.emplace(contract, [&](auto & a){
                    a.id = h_id;
                    a.date = date;
                    a.buyer = buyer;
                    a.seller = seller;
                    a.amount = amount;
                    a.price = price;
                    a.inverse = inverse;
                    a.payment = payment;
                    a.buyfee = buyfee;
                    a.sellfee = sellfee;
                    a.isbuy = is_buy;
                });

                // register event on historyall table
                historyall hall_table(contract, contract.value);
                uint64_t hall_id = hall_table.available_primary_key();
                hall_table.emplace(contract, [&](auto & a){
                    a.id = hall_table.available_primary_key();
                    a.key = h_id;
                    a.market = can_market;
                    a.date = date;
                });                

                // register event for activity log
                if (!inverted) {
                    aux_register_event(owner, name("transaction"), scope.to_string() + "|" + buyer.to_string() + "|" + seller.to_string() + "|" + amount.to_string() + "|" + payment.to_string() + "|" + price.to_string() );
                } else {
                    name actual_scope = aux_get_scope_for_tokens(A, B);
                    if (scope == actual_scope) {
                        actual_scope = aux_get_scope_for_tokens(B, A);
                    }
                    aux_register_event(owner, name("transaction"), actual_scope.to_string() + "|" + buyer.to_string() + "|" + seller.to_string() + "|" + payment.to_string() + "|" + amount.to_string() + "|" + inverse.to_string() );
                }
                
                // aux_trigger_event(amount.symbol.code(),  name("deal"), seller, buyer,  amount,  payment, price);
                // aux_trigger_event(payment.symbol.code(), name("deal"), buyer,  seller, payment, amount,  inverse);

                // find out last price
                asset last_price = price;
                last24hs l24table(contract, can_market);
                auto ptr = l24table.find(name("lastone").value);
                if (ptr != l24table.end()) {
                    last_price = ptr->price;
                }

                // calculate hour and label
                uint64_t sec = current_time_point().sec_since_epoch();
                uint64_t hour = sec / 3600;
                int  hora = hour % 24;
                name label = aux_create_label_for_hour(hora);
                
                // save table l24table (price & volume/h)
                ptr = l24table.find(label.value);
                if (ptr == l24table.end()) {
                    l24table.emplace(contract, [&](auto & a) {
                        a.label = label;
                        a.price = price;
                        a.inverse = inverse;
                        a.volume = payment;
                        a.amount = amount;
                        a.date = date;
                        a.hour = hour;
                        a.entrance = last_price;
                        a.min = price;
                        a.max = price;
                        if (last_price > a.max) a.max = last_price;
                        if (last_price < a.min) a.min = last_price;
                    });
                } else {
                    if (ptr->hour == hour) {
                        l24table.modify(*ptr, contract, [&](auto & a){
                            a.price = price;
                            a.inverse = inverse;
                            a.volume += payment;
                            a.amount += amount;
                            a.date = date;
                            if (price > a.max) a.max = price;
                            if (price < a.min) a.min = price;
                        });
                    } else {
                        check(ptr->hour < hour, "ERROR: inconsistency in hour property");
                        l24table.modify(*ptr, contract, [&](auto & a){
                            a.price = price;
                            a.inverse = inverse;
                            a.volume = payment;
                            a.amount = amount;
                            a.date = date;
                            a.hour = hour;
                            a.entrance = last_price;
                            a.min = price;
                            a.max = price;
                            if (last_price > a.max) a.max = last_price;
                            if (last_price < a.min) a.min = last_price;
                        });
                    }
                }


                ptr = l24table.find(label.value);
                asset volume = ptr->volume;
                asset total = ptr->amount;
                asset entrance = ptr->entrance;
                asset min = ptr->min;
                asset max = ptr->max;

                // save current block
                ptr = l24table.find(name("lastone").value);
                if (ptr == l24table.end()) {
                    l24table.emplace(contract, [&](auto & a) {
                        a.label = name("lastone");
                        a.price = price;
                        a.inverse = inverse;
                        a.volume = volume;
                        a.amount = total;
                        a.date = date;
                        a.hour = hour;
                        a.entrance = entrance;
                        a.min = min;
                        a.max = max;
                    });
                } else {
                    l24table.modify(*ptr, contract, [&](auto & a){
                        a.price = price;
                        a.inverse = inverse;
                        a.volume = volume;
                        a.amount = total;
                        a.date = date;
                        a.hour = hour;
                        a.entrance = entrance;
                        a.min = min;
                        a.max = max;
                    });
                }

                // save table l24table (price & volume/h)
                historyblock blocktable(contract, can_market);
                uint64_t bh_id = blocktable.available_primary_key();
                auto index = blocktable.template get_index<name("hour")>();
                auto bptr = index.find(hour);
                if (bptr == index.end()) {
                    blocktable.emplace(contract, [&](auto & a) {
                        a.id = bh_id;
                        a.price = price;
                        a.inverse = inverse;
                        a.volume = payment;
                        a.amount = amount;
                        a.date = date;
                        a.hour = hour;
                        a.entrance = last_price;
                        a.min = price;
                        a.max = price;
                        if (last_price > a.max) a.max = last_price;
                        if (last_price < a.min) a.min = last_price;
                    });
                } else {
                    blocktable.modify(*bptr, contract, [&](auto & a){
                        a.price = price;
                        a.inverse = inverse;
                        a.volume += payment;
                        a.amount += amount;
                        a.date = date;
                        if (price > a.max) a.max = price;
                        if (price < a.min) a.min = price;
                    });
                }


                // update deals (history table) & blocks (historyblock table) count for scope table
                ordersummary summary(contract, contract.value);
                auto orders_itr = summary.find(can_market);

                check(orders_itr != summary.end(), (string("Why is this entry missing? ") + scope.to_string() + string(" canonical market: ") + std::to_string((unsigned long)can_market)).c_str());
                summary.modify(*orders_itr, same_payer, [&](auto & a){
                    a.deals = h_id+1;
                    a.blocks = bh_id+1;

                    if (currency == a.pay) {
                        a.demand.ascurrency += 1;
                    } else {
                        a.supply.ascurrency += 1;
                    }
                });

                PRINT("vapaee::dex::record::aux_register_transaction_in_history() ...\n");
            }
        
        };     
    };
};
