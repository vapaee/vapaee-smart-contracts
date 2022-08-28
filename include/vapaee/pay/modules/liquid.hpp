#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pay/tables.hpp>
#include <vapaee/pay/errors.hpp>
#include <vapaee/pay/modules/hub.hpp>
#include <vapaee/dex/modules/global.hpp>
#include <vapaee/token/modules/utils.hpp>

namespace vapaee {
    namespace pay {
        namespace liquid {

            inline name get_self() {
                return vapaee::pay::contract;
            }


            // --- easing funtctions ---
            std::vector<name> get_supported_easing_functions() {
                return {
                    name("linear"),
                    name("quad.o")
                    // TODO: add "cubic.o" support
                    // ,name("cubic.o")
                };
            }

            bool is_easing_supported(name easing) {
                auto v = get_supported_easing_functions();
                return (std::count(v.begin(), v.end(), easing));
            }

            
            double get_position(const leakpools_table& leakpool) {
                time_point_sec start    = leakpool.start;
                time_point_sec end      = leakpool.end;
                time_point_sec rightnow = time_point_sec(current_time_point().sec_since_epoch());
                double elapsed          = (double) (rightnow.sec_since_epoch()-start.sec_since_epoch());
                double total            = (double) (end.sec_since_epoch()-start.sec_since_epoch());
                double result           = elapsed / total;

                // PRINT("get_position(",
                //     std::to_string((unsigned long)start.sec_since_epoch()),
                //     ",",
                //     std::to_string((unsigned long)end.sec_since_epoch()),
                //     ") rightnow: ", std::to_string((unsigned long)rightnow.sec_since_epoch()),
                //     " elapsed: ", std::to_string(elapsed),
                //     " total: ", std::to_string(total),
                //     " result: ", std::to_string(result),
                //     "\n");

                // check(false, "BOOOM");
                return result > 1.0 ? 1.0 : result;
            }

            asset tweenear_linear(double position, const leakpools_table& leakpool) {
                // https://github.com/mobius3/tweeny/blob/master/include/easing.h#L234
                return asset((uint64_t)(
                    leakpool.total.amount * position
                ), leakpool.total.symbol);
            }

            asset tweenear_quadOut(double position, const leakpools_table& leakpool) {
                // https://github.com/mobius3/tweeny/blob/master/include/easing.h#L256   
                return asset((uint64_t)(
                    -leakpool.total.amount * position * (position - 2)
                ), leakpool.total.symbol);
            }

            asset tweenear_cubicOut(double position, const leakpools_table& leakpool) {
                // https://github.com/mobius3/tweeny/blob/master/include/easing.h#L296

                double L = position;
                double Q = position * (2 - position);
                double C = (position * position * position + 1);
                PRINT("tweenear C: ", std::to_string(C), " Q: ", std::to_string(Q), " L: ", std::to_string(L), "\n");             

                return asset((uint64_t)(
                    leakpool.total.amount * (position * position * position + 1)
                ), leakpool.total.symbol);
            }

            // ---

            bool get_leakpool_for_id(
                bool create,
                uint64_t leakpool_id,
                leakpools_table& leakpool,
                const name& ram_payer,
                const char * error_code
            ) {

                leakpools liqs_table(get_self(), get_self().value);
                auto liq_ptr = liqs_table.find(leakpool_id);

                if (liq_ptr == liqs_table.end() && !create)  {
                    if (error_code) {
                        check(false, 
                            create_error_id1(
                                string(string(error_code) + " > ERR-GLPFI-01: the leakpools entry for the id was not found. (leakpool_id): ").c_str(),
                                leakpool_id
                            ).c_str()
                        );
                    } else {
                        return false;
                    }
                }

                if (liq_ptr == liqs_table.end() && create) {
                    liqs_table.emplace(ram_payer, [&](auto &a){ 
                        leakpool.id  = liqs_table.available_primary_key();
                        a.id         = leakpool.id;
                        a.admin      = leakpool.admin;
                        a.paygub     = leakpool.paygub;
                        a.title      = leakpool.title;
                        a.left       = leakpool.left;
                        a.total      = leakpool.total;
                        a.liquid     = leakpool.liquid;
                        a.allowed    = leakpool.allowed;
                        a.leaked     = leakpool.leaked;
                        a.delta      = leakpool.delta;
                        a.start      = leakpool.start;
                        a.end        = leakpool.end;
                        a.easing     = leakpool.easing;
                    });

                    return true;
                };

                if (liq_ptr != liqs_table.end() && create) {
                    liqs_table.modify(liq_ptr, ram_payer, [&](auto &a){
                        a.admin      = leakpool.admin;
                        a.paygub     = leakpool.paygub;
                        a.title      = leakpool.title;
                        a.total      = leakpool.total;
                        a.left       = leakpool.left;
                        a.liquid     = leakpool.liquid;
                        a.allowed    = leakpool.allowed;
                        a.leaked     = leakpool.leaked;
                        a.delta      = leakpool.delta;
                        a.start      = leakpool.start;
                        a.end        = leakpool.end;
                        a.easing     = leakpool.easing;
                    });

                    return true;
                };

                leakpool.id         = liq_ptr->id;
                leakpool.paygub     = liq_ptr->paygub;
                leakpool.admin      = liq_ptr->admin;
                leakpool.title      = liq_ptr->title;
                leakpool.total      = liq_ptr->total;
                leakpool.left       = liq_ptr->left;
                leakpool.liquid     = liq_ptr->liquid;
                leakpool.allowed    = liq_ptr->allowed;
                leakpool.leaked     = liq_ptr->leaked;
                leakpool.delta      = liq_ptr->delta;
                leakpool.start      = liq_ptr->start;
                leakpool.end        = liq_ptr->end;
                leakpool.easing     = liq_ptr->easing;

                return true;
            }

            void action_newleakpool(
                const name& admin,
                uint64_t payhub_id,
                const symbol_code& token,
                const string& title,
                const asset& liquidity,
                const asset& issue_allaw,
                const name& easing,
                uint32_t epoch_start,
                uint32_t epoch_end
            ) {
                PRINT("vapaee::pay::liquid::action_newleakpool()\n");
                PRINT(" admin: ", admin.to_string(), "\n");
                PRINT(" payhub_id: ", std::to_string((long)payhub_id), "\n");
                PRINT(" token: ", token.to_string(), "\n");
                PRINT(" title: ", title.c_str(), "\n");
                PRINT(" liquidity: ", liquidity.to_string(), "\n");
                PRINT(" issue_allaw: ", issue_allaw.to_string(), "\n");
                PRINT(" easing: ", easing.to_string(), "\n");
                PRINT(" epoch_start: ", std::to_string((long)epoch_start), "\n");
                PRINT(" epoch_end: ", std::to_string((long)epoch_end), "\n");

                // require admin auth
                require_auth(admin);
                name ram_payer = admin;
                
                // this call already checks if token is legit
                asset supply = vapaee::dex::token::get_token_supply(token);
                check(supply.symbol == liquidity.symbol,
                    create_error_asset2("ERR-ANLP-03: liquidity symbol is not the same as the token supply. (supply, liquidity):", 
                    supply, liquidity).c_str());
                
                name contract = vapaee::dex::utils::get_contract_for_token(supply.symbol.code());

                asset zero              = asset(0, supply.symbol);
                asset leaked            = zero;
                asset allowed           = issue_allaw;
                asset liquid            = liquidity;
                asset total             = allowed + liquid;
                asset left              = total;
                time_point_sec start    = time_point_sec(epoch_start);
                time_point_sec end      = time_point_sec(epoch_end);
                time_point_sec rightnow = vapaee::dex::global::get_now_time_point_sec();

                if (liquidity.amount > 0) {
                    // debit liquidity from owner
                    string memo = string("adding ") + liquidity.to_string() + " into pool ("+std::to_string((long)payhub_id)+") liquidity.";
                    vapaee::token::utils::send_debit_to_owner(admin, get_self(), liquidity, memo);
                }

                if (issue_allaw.amount > 0) {
                    // Check symbol integrity
                    check(issue_allaw.symbol == supply.symbol,
                        create_error_asset2(
                            "ERR-ANLP-01: issue_allaw and token supply differ in symbol. (token supply, issue allowed)",
                            supply, 
                            issue_allaw
                        ).c_str()
                    );

                    // Check vapaeepayhub is able to issue tokens and send them
                    name contract = vapaee::dex::utils::get_contract_for_token(supply.symbol.code());
                    asset test = asset(1, supply.symbol);
                    vapaee::token::utils::send_issue_tokens(test, string("testing issue action from vapaeepayhub"), contract);
                    vapaee::token::utils::send_transfer_tokens(get_self(), admin, test, "returning the asset from vapaeepayhub issue test", contract);
                }

                // check paygub exists
                payhubs_table payhub;
                vapaee::pay::hub::get_payhub_for_id(false, payhub_id, payhub, ram_payer, "ERR-ANLP-02");

                // check time points
                check(epoch_start < epoch_end,
                    create_error_time2("ERR-ANLP-04: epoch_start must be lower than epoch_end. (epoch_start, epoch_end):", 
                    start, end).c_str()
                );
                check(rightnow.sec_since_epoch() < end.sec_since_epoch(),
                    create_error_time2("ERR-ANLP-05: 'right now' must be lower than epoch_end. (right now, epoch_end):", 
                    rightnow, end).c_str()
                );

                // check chosen easing
                bool supported = is_easing_supported(easing);
                if (!supported) {
                    
                    // create the string to show supported list for a propper error info
                    string supported_list = "(";
                    vector<name> list = get_supported_easing_functions();
                    for (int i=0; i<list.size(); i++) {
                        if (i>0) supported_list += ", ";
                        supported_list += list[i].to_string();
                    }
                    supported_list += ")";

                    check(false,
                        create_error_string2("ERR-ANLP-06: easing function not supported. (supported, recived): ",
                        supported_list,
                        easing.to_string()).c_str()
                    );
                }
                
                // Copy all data and save
                leakpools_table leakpool;
                leakpool.admin   = admin;
                leakpool.paygub  = payhub_id;
                leakpool.title   = title;
                leakpool.total   = total;
                leakpool.left    = left;
                leakpool.liquid  = liquid;
                leakpool.allowed = allowed;
                leakpool.leaked  = leaked;
                leakpool.start   = start;
                leakpool.end     = end;
                leakpool.easing  = easing;
                           
                get_leakpool_for_id(true, MAX_VALUE, leakpool, ram_payer, "ERR-ANLP-07");
            }

            void action_udpleakpool(
                name admin,
                uint64_t leakpool_id,
                string title,
                asset issue_allaw_more
            ) {
                PRINT("vapaee::pay::liquid::action_udpleakpool()\n");
                PRINT(" admin: ", admin.to_string(), "\n");
                PRINT(" leakpool_id: ", std::to_string((long)leakpool_id), "\n");
                PRINT(" title: ", title.c_str(), "\n");
                PRINT(" issue_allaw_more: ", issue_allaw_more.to_string(), "\n");

                require_auth(admin);
                name ram_payer = admin;
                
                leakpools_table leakpool;
                get_leakpool_for_id(false, leakpool_id, leakpool, ram_payer, "ERR-AULP-01");

                // calculate
                asset allowed = leakpool.allowed + issue_allaw_more;
                asset total = leakpool.total + issue_allaw_more;
                asset left  = leakpool.liquid + allowed;
                uint32_t time_total = leakpool.end.sec_since_epoch() - leakpool.start.sec_since_epoch();
                uint32_t time_plus = 0;
                if (leakpool.total.amount > 0) {
                    time_plus = (uint32_t) ((double) time_total * ( (double) issue_allaw_more.amount / (double) leakpool.total.amount ));
                }                
                time_point_sec end = time_point_sec(leakpool.end.sec_since_epoch() + time_plus);

                if (title.length() == 0) { title = leakpool.title; }

                // update 
                leakpool.title   = title;
                leakpool.total   = total;
                leakpool.left    = left;
                leakpool.allowed = allowed;
                leakpool.end     = end;

                // save
                get_leakpool_for_id(true, leakpool_id, leakpool, ram_payer, "ERR-AULP-02");
            }

            void action_leakpool(
                uint64_t leakpool_id
            ) {
                PRINT("vapaee::pay::liquid::action_leakpool()\n");
                PRINT(" leakpool_id: ", std::to_string((long)leakpool_id), "\n");

                name ram_payer = same_payer;
                leakpools_table leakpool;
                get_leakpool_for_id(false, leakpool_id, leakpool, get_self(), "ERR-ALP-01");

                // calculate
                asset leaked  = leakpool.leaked;
                asset left    = leakpool.left;
                asset allowed = leakpool.allowed;
                asset liquid  = leakpool.liquid;

                // calculate leak
                double position = get_position(leakpool);
                if (position < 1.0) {
                    switch(leakpool.easing.value) {
                        case name("linear").value:  { leaked = tweenear_linear(position, leakpool);   break; }
                        case name("quad.o").value:  { leaked = tweenear_quadOut(position, leakpool);  break; }
                        case name("cubic.o").value: { leaked = tweenear_cubicOut(position, leakpool); break; }
                        default: {
                            check(false,
                                create_error_name1("ERR-ALP-02: easing function not supported: ",
                                leakpool.easing).c_str()
                            );                        
                        }
                    }
                } else {
                    // this avoid leaking more than we have leading to an error
                    leaked = leakpool.total;
                }
                

                asset delta = leaked - leakpool.leaked;
                asset remaining = delta;


                PRINT(" > position: ", std::to_string(position), "\n");
                PRINT(" > leaked: ", leaked.to_string(), "\n");
                PRINT(" > delta: ", delta.to_string(), "\n");

                // If theres no funds to leak then just return with no error
                if (delta.amount == 0) return;

                // try to extract from liquidity
                asset liquid_extract = asset(0, delta.symbol);
                if (liquid.amount > 0) {
                    if (liquid.amount > delta.amount) {
                        // there is enough liquidity. We take all delta from liquidity
                        liquid_extract = delta;
                    } else {
                        // there is not enough liquidity so we take all liquidity and we need more
                        liquid_extract = liquid;
                    }
                    liquid -= liquid_extract;
                    remaining -= liquid_extract;
                }

                // if there is not enough liquidity to cover the leak.
                // then we need issue some tokens
                if (remaining.amount > 0) {
                    if (leakpool.allowed.amount > 0) {                        
                        // issue tokens
                        name contract = vapaee::dex::utils::get_contract_for_token(remaining.symbol.code());
                        string memo = string("vapaeepayhub need to issue tokens to leak from liquidity pool: ") + std::to_string((long)leakpool_id);
                        vapaee::token::utils::send_issue_tokens(remaining, memo, contract);
                        allowed -= remaining;
                    } else {
                        // No more money 
                        PRINT(" > leakpool.allowed: ", leakpool.allowed.to_string(), "\n");
                        check(false, "ERR-ALP-03: there's no more money in the liquidity pool");
                    }
                }
                
                left -= delta;

                // update
                leakpool.left    = left;
                leakpool.liquid  = liquid;
                leakpool.allowed = allowed;
                leakpool.leaked  = leaked;
                leakpool.delta   = delta;

                // save
                get_leakpool_for_id(true, leakpool_id, leakpool, ram_payer, "ERR-ALP-04");

                // deliver the leaked assets
                vapaee::pay::hub::pay_to_payhub(delta, leakpool.paygub, true);
            }

            // auxiliar ---
            uint64_t extract_leakpool_id(const string& target) {
                return std::atol(target.c_str());
            }

            // handler -----
            void handle_adding_liquidity(const asset& quantity, string& target) {
                PRINT("vapaee::pay::liquid::handle_adding_liquidity()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" target: ", target.c_str(), "\n");

                name ram_payer = same_payer;
                uint64_t leakpool_id = extract_leakpool_id(target);

                leakpools_table leakpool;
                get_leakpool_for_id(false, leakpool_id, leakpool, get_self(), "ERR-HAL-01");

                // calculate
                uint32_t time_total = leakpool.end.sec_since_epoch() - leakpool.start.sec_since_epoch();
                uint32_t time_plus = 0;
                if (leakpool.total.amount > 0) {
                    time_plus = (uint32_t) ((double) time_total * ( (double) quantity.amount / (double) leakpool.total.amount ));
                }
                time_point_sec end   = time_point_sec(leakpool.end.sec_since_epoch() + time_plus);
                time_point_sec start = leakpool.start;
                asset liquid         = leakpool.liquid + quantity;
                asset total          = leakpool.total + quantity;
                asset left           = leakpool.left + quantity;

                if (leakpool.left.amount == 0) {
                    // In this case we have an empty pool.
                    // Then we reset the pool.
                    total = quantity;
                    liquid = quantity;
                    left = quantity;
                    start = vapaee::dex::global::get_now_time_point_sec();
                    end   = time_point_sec(start.sec_since_epoch() + time_plus);
                }

                leakpool.total   = total;
                leakpool.liquid  = liquid;
                leakpool.left    = left;
                leakpool.end     = end;
                leakpool.start   = start;

                get_leakpool_for_id(true, leakpool_id, leakpool, ram_payer, "ERR-HAL-02");                

            }

        };     
    };
};
