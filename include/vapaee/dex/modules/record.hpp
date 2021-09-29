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
                    case  0: return "h.zero"_n;
                    case  1: return "h.one"_n;
                    case  2: return "h.two"_n;
                    case  3: return "h.three"_n;
                    case  4: return "h.four"_n;
                    case  5: return "h.five"_n;
                    case  6: return "h.six"_n;
                    case  7: return "h.seven"_n;
                    case  8: return "h.eight"_n;
                    case  9: return "h.nine"_n;
                    case 10: return "h.ten"_n;
                    case 11: return "h.eleven"_n;
                    case 12: return "h.twelve"_n;
                    case 13: return "h.thirteen"_n;
                    case 14: return "h.fourteen"_n;
                    case 15: return "h.fifteen"_n;
                    case 16: return "h.sixteen"_n;
                    case 17: return "h.seventeen"_n;
                    case 18: return "h.eighteen"_n;
                    case 19: return "h.nineteen"_n;
                    case 20: return "h.twenty"_n;
                    case 21: return "h.twentyone"_n;
                    case 22: return "h.twentytwo"_n;
                    case 23: return "h.twentythree"_n;
                    default: {
                        check(false, string("ERROR: bad hour") + std::to_string(hh));
                    }
                }
            }


            void aux_register_transaction_in_history(
                bool inverted,
                name buyer, name seller,
                asset price,   // unit price
                asset inverse, // inverse of unit price in commodity sym
                asset payment, // units of commodity
                asset amount,  // total price
                asset buyfee, asset sellfee
            ) {
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
                uint128_t index = aux_get_canonical_index_for_symbols(A, B);

                
                bool is_buy = false;
                if (index == symbols_get_index(B, A)) {
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
                    aux_register_event(
                        owner,
                        "transaction"_n,
                        aux_get_market_repr(market) + "|" +
                        buyer.to_string() + "|" + 
                        seller.to_string() + "|" +
                        amount.to_string() + "|" +
                        payment.to_string() + "|" +
                        price.to_string()
                    );
                } else {
                    uint128_t actual_index = symbols_get_index(A, B);
                    if (index == actual_index)
                        actual_index = symbols_get_index(B, A);
        
                    aux_register_event(
                        owner,
                        "transaction"_n,
                        aux_get_market_repr(market) + "|" +
                        buyer.to_string() + "|" +
                        seller.to_string() + "|" +
                        payment.to_string() + "|" +
                        amount.to_string() + "|" +
                        inverse.to_string()
                    );
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
                auto hour_index = blocktable.get_index<"hour"_n>();
                auto bptr = hour_index.find(hour);
                if (bptr == hour_index.end()) {
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

                check(
                    orders_itr != summary.end(),
                    (string("Why is this entry missing? ") + 
                     aux_get_market_repr(market) +
                     string(" canonical market: ") +
                     aux_get_market_repr(can_market))
                );
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

            void action_record_pool_swap(
                name sender,
                name recipient,
                asset rate,
                asset sent, asset result
            ) {
                symbol_code A = sent.symbol.code();
                symbol_code B = result.symbol.code();

                uint128_t index = symbols_get_index(A, B);
                uint128_t index_can = aux_get_canonical_index_for_symbols(A, B);

                bool inverted = index != index_can;

                asset inverse = vapaee::utils::inverse(rate, sent.symbol);
                aux_register_transaction_in_history(
                    inverted,
                    recipient, sender,
                    rate,
                    inverse,
                    sent,
                    result,
                    asset(0, symbol("NONE", 1)),
                    asset(0, symbol("NONE", 1)));
            }
        
        };     
    };
};
