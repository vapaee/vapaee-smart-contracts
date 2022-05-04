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
                        check(false, create_error_string1(ERROR_ACLFH_1, std::to_string(hh)));
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
                PRINT("vapaee::dex::record::aux_record_in_market_history()\n");
                                                                        // ACTION: order
                                                                        //  owner: bob                    //  owner: bob
                                                                        //  type: sell                    //  type: buy
                                                                        //  total: 10.00000000 CNT        //  total: 10.00000000 CNT
                                                                        //  price: 0.29000000 DEX         //  price: 0.42000000 DEX
                                                                        //  client: 0                     //  client: 0                  
                                                                        // ------------------------------------
                PRINT(" type: ", type.to_string(), "\n");               //  type: sell                    // type: buy
                PRINT(" buyer: ", buyer.to_string(), "\n");             //  buyer: alice                  // buyer: alice
                PRINT(" seller: ", seller.to_string(), "\n");           //  seller: bob                   // seller: bob   
                PRINT(" price: ", price.to_string(), "\n");             //  price: 0.29000000 DEX         // price: 0.42000000 DEX
                PRINT(" inverse: ", inverse.to_string(), "\n");         //  inverse: 3.44827586 CNT       // inverse: 2.38095238 CNT
                PRINT(" payment: ", payment.to_string(), "\n");         //  payment: 2.90000000 DEX       // payment: 4.20000000 DEX
                PRINT(" amount: ", amount.to_string(), "\n");           //  amount: 10.00000000 CNT       // amount: 10.00000000 CNT
                PRINT(" buyfee: ", buyfee.to_string(), "\n");           //  buyfee: 0.00327680 CNT        // buyfee: 0.00137625 DEX
                PRINT(" sellfee: ", sellfee.to_string(), "\n");         //  sellfee: 0.00475136 DEX       // sellfee: 0.01638400 CNT
        
                check(price.symbol == payment.symbol, create_error_asset2(ERROR_ARIMH_1, price, payment));
                check(inverse.symbol == amount.symbol, create_error_asset2(ERROR_ARIMH_2, inverse, amount));

                time_point_sec date = get_now_time_point_sec();
                name tmp_name;
                asset tmp_asset;
                asset tmp_pay;
                name owner = seller;
                symbol_code currency = price.symbol.code();
                
                symbol_code A = amount.symbol.code();
                symbol_code B = payment.symbol.code();

                bool is_buy = (type == name("buy"));

                // decide rampayer
                name rampayer = buyer;
                if (!has_auth(rampayer)) rampayer = seller;
                if (!has_auth(rampayer)) rampayer = vapaee::pool::contract;
                if (!has_auth(rampayer)) rampayer = vapaee::book::contract;
                if (!has_auth(rampayer)) rampayer = vapaee::dex::contract;
                // --------------------------------------------------

                uint64_t market = aux_get_market_id(A, B);
                uint64_t can_market = aux_get_canonical_market_id(A, B);

                // Get the instance of cannonical market 
                markets mtable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto market_itr = mtable.find(can_market);
                check(market_itr != mtable.end(), 
                    create_error_id1(ERROR_ARIMH_3, can_market));

                // check price is the same symbol as currency of the market
                check(price.symbol.code() == market_itr->currency, 
                    create_error_string2(ERROR_ARIMH_4, price.to_string(), market_itr->currency.to_string()));

                // register event on history table
                history table(contract, can_market);
                uint64_t h_id = table.available_primary_key();
                table.emplace(rampayer, [&](auto & a){
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
                hall_table.emplace(rampayer, [&](auto & a){
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
                    l24table.emplace(rampayer, [&](auto & a) {
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
                        l24table.modify(*ptr, rampayer, [&](auto & a){
                            a.price = price;
                            a.inverse = inverse;
                            a.volume += payment;
                            a.amount += amount;
                            a.date = date;
                            if (price > a.max) a.max = price;
                            if (price < a.min) a.min = price;
                        });
                    } else {
                        check(ptr->hour < hour, create_error_id2(ERROR_ARIMH_5, ptr->hour, hour));
                        l24table.modify(*ptr, rampayer, [&](auto & a){
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
                    l24table.emplace(rampayer, [&](auto & a) {
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
                    l24table.modify(*ptr, rampayer, [&](auto & a){
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
                    blocktable.emplace(rampayer, [&](auto & a) {
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
                    blocktable.modify(*bptr, rampayer, [&](auto & a){
                        a.price = price;
                        a.inverse = inverse;
                        a.volume += payment;
                        a.amount += amount;
                        a.date = date;
                        if (price > a.max) a.max = price;
                        if (price < a.min) a.min = price;
                    });
                }

                PRINT("vapaee::dex::record::aux_record_in_market_history() ...\n");
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
                PRINT("vapaee::dex::record::action_record_book_deal()\n");
                PRINT("  type: ", type.to_string(), "\n");
                PRINT("  buyer: ", buyer.to_string(), "\n");
                PRINT("  seller: ", seller.to_string(), "\n");
                PRINT("  price: ", price.to_string(), "\n");
                PRINT("  inverse: ", inverse.to_string(), "\n");
                PRINT("  payment: ", payment.to_string(), "\n");
                PRINT("  amount: ", amount.to_string(), "\n");
                PRINT("  buyfee: ", buyfee.to_string(), "\n");
                PRINT("  sellfee: ", sellfee.to_string(), "\n");

                uint64_t market = aux_get_market_id(amount.symbol.code(), payment.symbol.code());

                // register event for activity log
                aux_register_event(
                    seller,
                    name("deal"),
                    aux_get_market_name(market) + "|" +
                    buyer.to_string() + "|" + 
                    seller.to_string() + "|" +
                    amount.to_string() + "|" +
                    payment.to_string() + "|" +
                    price.to_string()
                );

                aux_record_in_market_history(type, buyer, seller, price, inverse, payment, amount, buyfee, sellfee);                
            }

            void action_record_pool_swap(
                name recipient,
                name converter,
                asset rate,
                asset sent,
                asset result,
                asset fee
            ) {
                PRINT("vapaee::dex::record::action_record_pool_swap()\n");
                PRINT("recipient: " + recipient.to_string() + "\n");
                PRINT("converter: " + converter.to_string() + "\n");
                PRINT("rate: " + rate.to_string() + "\n");
                PRINT("sent: " + sent.to_string() + "\n");
                PRINT("result: " + result.to_string() + "\n");
                PRINT("fee: " + fee.to_string() + "\n");
                
                symbol_code A = sent.symbol.code();
                symbol_code B = result.symbol.code();

                uint64_t market = aux_get_market_id(A, B);
                uint64_t can_market = aux_get_canonical_market_id(A, B);

                vapaee::dex::market::aux_check_converter_is_valid(can_market, converter);
                vapaee::dex::market::aux_update_converter_state(can_market, converter);

                bool inverted = market != can_market;

                asset inverse = vapaee::utils::inverse(rate, sent.symbol);
                asset nofee = aux_extend_asset(asset(0, rate.symbol));
                aux_register_event(
                    recipient,
                    name("swap"),
                    sent.to_string() + "|" + result.to_string() + "|" + rate.to_string() );
                
                if (inverted) {
                    aux_record_in_market_history(
                        name("buy"),                             // type
                        recipient,                               // buyer
                        converter,                               // seller
                        aux_extend_asset(inverse),               // price
                        aux_extend_asset(rate),                  // inverse
                        aux_extend_asset(sent),                  // payment
                        aux_extend_asset(result),                // amount
                        aux_extend_asset(fee),                   // buyfee
                        nofee                                    // sellfee
                    );
                } else {
                    aux_record_in_market_history(
                        name("sell"),                            // type
                        converter,                               // buyer
                        recipient,                               // seller
                        aux_extend_asset(rate),                  // price
                        aux_extend_asset(inverse),               // inverse
                        aux_extend_asset(result),                // payment
                        aux_extend_asset(sent),                  // amount
                        nofee,                                   // buyfee
                        aux_extend_asset(fee)                    // sellfee
                    );
                }



            }

            void action_update_pool_swap_state(
                uint64_t market,
                name converter
            ) {
                PRINT("vapaee::dex::record::action_update_pool_swap_state()\n");
                PRINT("market: " + std::to_string(market) + "\n");
                PRINT("converter: " + converter.to_string() + "\n");

                vapaee::dex::market::aux_check_converter_is_valid(market, converter);
                vapaee::dex::market::aux_update_converter_state(market, converter);
            }
        
        };     
    };
};
