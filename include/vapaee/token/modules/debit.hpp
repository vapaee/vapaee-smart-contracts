#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/token/errors.hpp>
#include <vapaee/dex/modules/global.hpp>
#include <vapaee/token/modules/standard.hpp>
#include <vapaee/token/modules/wrapper.hpp>

namespace vapaee {
    namespace token {
        namespace debit {

            inline name get_self() {
                return vapaee::token::contract;
            }

            // TODO: Refactor this code that has been copied from wrapper.hpp
            void send_transfer_tokens(const name& from, const name& to, const asset& quantity, const string& memo, const name& contract  ) {
                action(
                    permission_level{get_self(), "active"_n},
                    contract,
                    "transfer"_n,
                    make_tuple(
                        from,
                        to,
                        quantity,
                        memo
                    )
                ).send();
            }

            void check_token_debitable(const symbol_code& token, const name& token_contract, string error) {
                PRINT("vapaee::token::debit::check_token_debitable()\n");
                bool foreign = vapaee::token::wrapper::is_token_registered_as_foreign(token);
                bool is_vapaee = token_contract == vapaee::token::contract;

                if (is_vapaee) {
                    name foreign_contract = vapaee::token::wrapper::get_token_foreign_contract(token);
                    check(!foreign, error + " Reason: Token is registered as 'foreign' with contract account ["+foreign_contract.to_string()+"]. Call this same action but using the that contract account instead if using vapaeetokens.");
                } else {
                    // we check it is already registered as foreign
                    check(foreign, error + " Reason: Token is not registered in vapaeetokens as foreign token yet. Try to transfer any amount to vapaeetokens with memo 'deposit' and call this action again.");   
                }

                // only accept registered tokens
                asset supply = vapaee::token::wrapper::get_token_supply(token, 
                    create_error_symcode1("ERR-CTD-01: could not get toekn supply: ", token));
                PRINT(" supply: ",supply.to_string(),"\n");
                vapaee::dex::security::aux_check_token_ok(supply.symbol, token_contract, error + " Reason: Token is not registered on telosmaindex");

            }

            void action_manage_debit(
                const name& action, 
                const name& owner, 
                const name& collector, 
                const asset& max_total, 
                const asset& max_per_debit, 
                const double& max_percent, 
                const uint64_t& sec_to_expire
            ) {
                PRINT("vapaee::token::debit::action_manage_debit()\n");
                PRINT(" action: ", action.to_string(), "\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" collector: ", collector.to_string(), "\n");
                PRINT(" max_total: ", max_total.to_string(), "\n");
                PRINT(" max_per_debit: ", max_per_debit.to_string(), "\n");
                PRINT(" max_percent: ", std::to_string(max_percent), "\n");
                PRINT(" sec_to_expire: ",std::to_string((unsigned long)sec_to_expire)," \n");

                time_point_sec _now = vapaee::dex::global::get_now_time_point_sec();
                time_point_sec expiration = vapaee::dex::global::get_N_seconds_from_point_sec(_now, sec_to_expire);

                if (sec_to_expire == 0) {
                    expiration = time_point_sec(0);
                }

                PRINT(" expiration: ",std::to_string((unsigned long)expiration.utc_seconds)," \n");

                require_auth(owner);
                name ram_payer = owner;

                debits debits_table(get_self(), owner.value);
                auto collector_ptr = debits_table.find(collector.value);

                if (action == name("add")) {
                    check(collector_ptr == debits_table.end(),
                        create_error_name1("ERR-AMD-1: can't add colletor because already exists. Call this action with 'update' action instead:", collector).c_str());

                    debits_table.emplace(ram_payer, [&](auto &a){
                        a.collector = collector;
                        a.max_total = max_total;
                        a.max_per_debit = max_per_debit;
                        a.max_percent = max_percent;
                        a.expiration = expiration;
                    });
                    return;
                }

                if (action == name("update")) {
                    check(collector_ptr != debits_table.end(),
                        create_error_name1("ERR-AMD-2: can't update. Entry not found for collector:", collector).c_str());

                    debits_table.modify(*collector_ptr, ram_payer, [&](auto &a){
                        a.max_total = max_total;
                        a.max_per_debit = max_per_debit;
                        a.max_percent = max_percent;
                        a.expiration = expiration;
                    });
                    return;
                }

                if (action == name("remove")) {
                    check(collector_ptr != debits_table.end(),
                        create_error_name1("ERR-AMD-3: can't remove. Entry not found for collector:", collector).c_str());

                    debits_table.erase(*collector_ptr);
                    return;
                }

                check(false, create_error_name1("ERR-AMD-4: action mus be one of the following: 'add', 'update' or 'remove'. Got: ", action));
            }

            void action_debit(const name& owner, const name& collector, const asset& quantity, const string& memo) {
                PRINT("vapaee::token::debit::action_debit()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" collector: ", collector.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");

                require_auth(collector);
                name ram_payer = collector;

                debits debits_table(get_self(), owner.value);
                auto debit_ptr = debits_table.find(collector.value);

                check(debit_ptr != debits_table.end(),
                    create_error_name2("ERR-AD-1: No debit access allowed for collector over owner:", collector, owner).c_str());

                asset balance = vapaee::token::standard::get_balance(owner, quantity.symbol);

                // Check max per debit
                if (debit_ptr->max_per_debit.amount > 0) {
                    // This debit has a maximun amount per debit.
                    // So quantity must be lower than this max
                    check(quantity.amount <= debit_ptr->max_per_debit.amount,
                        create_error_asset2("ERR-AD-2: Maximun amount per debit reached. [maximun debit allowed, current debit]",
                        debit_ptr->max_per_debit,
                        quantity
                    ).c_str());                    
                }

                // Check max per debit
                if (debit_ptr->max_percent > 0) {
                    // This debit has a maximun percent over the current balance per debit.
                    // So quantity must be lower than balance * max_percent
                    asset percent = balance;
                    percent.amount = debit_ptr->max_percent * balance.amount;

                    check(quantity.amount <= percent.amount,
                        create_error_asset2("ERR-AD-3: Maximun percent over balance per debit reached. [maximun debit allowed, current debit]",
                        percent,
                        quantity
                    ).c_str());                    
                }


                // check if symbols are equals
                check(quantity.symbol == debit_ptr->max_total.symbol,
                    create_error_asset2("ERR-AD-4: Debit symbol mismatch. [allowed, current debit]:",
                    debit_ptr->max_total,
                    quantity
                ).c_str());

                // Check and update Total Maximun 
                if (debit_ptr->max_total.amount > 0) {
                    // This debit has a maximun amount to debit in total.
                    // So we must update value substracting the current quantity
                    check(quantity.amount <= debit_ptr->max_total.amount,
                        create_error_asset2("ERR-AD-5: Maximun total debit amount reached. [maximun left, current debit]: ",
                        debit_ptr->max_total,
                        quantity
                    ).c_str());

                    if (quantity.amount == debit_ptr->max_total.amount) {
                        debits_table.erase(debit_ptr);
                    } else {
                        debits_table.modify(*debit_ptr, ram_payer, [&](auto &a){
                            a.max_total -= quantity;
                        });

                        check(false,
                            create_error_asset2("ERR-AD-6: For now is not allowed to debit partially. It must be totallyconsumed. [available, consumed]: ",
                            debit_ptr->max_total,
                            quantity
                        ).c_str());
                    }
                }

                // send un transfer from owner to collector por quantity con un memo que indique que es un débito a nombre de quien y replique el memo original.
                string new_memo = string("vapaeetokens debit service: ") + memo;
                
                send_transfer_tokens(owner, collector, quantity, new_memo.c_str(), get_self());

                
                PRINT("vapaee::token::debit::action_debit()...\n");
            }

        };     
    };
};
