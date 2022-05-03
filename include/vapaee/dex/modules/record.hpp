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

            void aux_register_event(name user, name event, string params) {
                PRINT("vapaee::dex::record::aux_register_event()\n");
                PRINT(" user: ", user.to_string(), "\n");
                PRINT(" event: ", event.to_string(), "\n");
                PRINT(" params: ", params.c_str(), "\n");
                time_point_sec date = get_now_time_point_sec();
                
                // the first entry of the table has a special meaning.
                // It serves as a counter
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
                return "."_n;
            }

            void aux_record_in_market_history(
                    name type,
                    name buyer,
                    name seller,
                    asset price,   // unit price
                    asset inverse, // inverse of unit price in commodity sym
                    asset payment, // units of commodity
                    asset amount,  // total price
                    asset buyfee, 
                    asset sellfee
                ) {
                PRINT("vapaee::dex::record::action_record_book_deal()\n");
                                                                        // ACTION: order
                                                                        //  owner: kate                   //  owner: kate
                                                                        //  type: sell                    //  type: buy
                                                                        //  total: 10.00000000 CNT        //  total: 10.00000000 CNT
                                                                        //  price: 0.29000000 TLOS        //  price: 0.40000000 TLOS
                                                                        //  client: 0                     //  client: 0                  
                                                                        // ------------------------------------
                PRINT(" type: ", type.to_string(), "\n");               //  type: sell                    // type: buy
                PRINT(" buyer: ", buyer.to_string(), "\n");             //  buyer: alice                  // buyer: alice
                PRINT(" seller: ", seller.to_string(), "\n");           //  seller: kate                  // seller: kate   
                PRINT(" price: ", price.to_string(), "\n");             //  price: 0.29000000 TLOS        // price: 
                PRINT(" inverse: ", inverse.to_string(), "\n");         //  inverse: 3.44827586 CNT       // inverse: 
                PRINT(" payment: ", payment.to_string(), "\n");         //  payment: 2.90000000 TLOS      // payment: 
                PRINT(" amount: ", amount.to_string(), "\n");           //  amount: 10.00000000 CNT       // amount: 
                PRINT(" buyfee: ", buyfee.to_string(), "\n");           //  buyfee: 0.10000000 CNT        // buyfee: 
                PRINT(" sellfee: ", sellfee.to_string(), "\n");         //  sellfee: 0.07250000 TLOS      // sellfee:
        
                check(price.symbol == payment.symbol, "ERROR: price.symbol != payment.symbol");
                check(inverse.symbol == amount.symbol, "ERROR: inverse.symbol != amount.symbol");

                time_point_sec date = get_now_time_point_sec();
                name tmp_name;
                asset tmp_asset;
                asset tmp_pay;
                name owner = seller;
                symbol_code currency = price.symbol.code();
                
                symbol_code A = amount.symbol.code();
                symbol_code B = payment.symbol.code();

                bool is_buy = (type == name("buy"));

                // --------------------------------------------------

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

                    // TODO: re-view this code
                    if (currency == a.pay) {
                        a.demand.ascurrency += 1;
                    } else {
                        a.supply.ascurrency += 1;
                    }
                });

                PRINT("vapaee::dex::record::action_record_book_deal() ...\n");
            }

            void action_record_book_deal(
                    name type,
                    name buyer,
                    name seller,
                    asset price,   // unit price
                    asset inverse, // inverse of unit price in commodity sym
                    asset payment, // units of commodity
                    asset amount,  // total price
                    asset buyfee, 
                    asset sellfee
            ) {
                uint64_t market = aux_get_market_id(amount.symbol.code(), payment.symbol.code());

                // register event for activity log
                aux_register_event(
                    seller,
                    name("deal"),
                    aux_get_market_repr(market) + "|" +
                    buyer.to_string() + "|" + 
                    seller.to_string() + "|" +
                    amount.to_string() + "|" +
                    payment.to_string() + "|" +
                    price.to_string()
                );

                aux_record_in_market_history(type, buyer, seller, price, inverse, payment, amount, buyfee, sellfee);
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

                aux_register_event(
                    recipient,
                    name("swap"),
                    sent.to_string() + "|" + result.to_string() + "|" + rate.to_string() );
                
                // TODO: re-view this code
                aux_record_in_market_history(
                    inverted ? name("buy") : name("sell"),
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
