#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/book/constants.hpp>
#include <vapaee/book/errors.hpp>
#include <vapaee/book/tables.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/record.hpp>
#include <vapaee/dex/modules/market.hpp>
#include <vapaee/dex/modules/client.hpp>
#include <vapaee/dex/modules/security.hpp>

namespace vapaee {
    namespace dex {
        namespace utils {
            asset aux_extend_asset(const asset & quantity);
            name get_contract_for_token(symbol_code sym);
            asset aux_get_real_asset(const asset & quantity_extended);
        };
        namespace security {
            void aux_check_token_ok(const symbol& sym, name tokencontract, string error_code);
            bool aux_is_token_blacklisted(const symbol_code &sym_code, name tokencontract);
            bool aux_is_token_blacklisted(const symbol_code &sym_code);
        };
    };
};


namespace vapaee {
    namespace book {

        namespace deposit {


            name get_self() {
                return vapaee::book::contract;
            }

            void aux_substract_deposits(name owner, const asset & amount) {
                PRINT("vapaee::book::deposit::aux_substract_deposits()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" amount: ", amount.to_string(), "\n");

                deposits depositstable(get_self(), owner.value);
                auto itr = depositstable.find(amount.symbol.code().raw());
                check(itr != depositstable.end(),
                            create_error_asset1(ERROR_ASD_1, amount).c_str());

                //check(itr->amount.symbol == amount.symbol,
                //    create_error_asset2(ERROR_ASD_2, itr->amount, amount).c_str());
                asset extended = vapaee::dex::utils::aux_extend_asset(amount);
                PRINT(" extended: ", extended.to_string(), "\n");
                if (itr->amount == extended) {
                    PRINT("  (itr->amount == amount): (",  amount.to_string(), ")\n");
                    PRINT(" -> depositstable.erase() : \n");
                    depositstable.erase(itr);
                } else {
                    PRINT("  (itr->amount > amount): (", itr->amount.to_string(), " > ", amount.to_string(),  ")\n");
                    check(itr->amount > extended,
                            create_error_asset2(ERROR_ASD_3, extended, itr->amount).c_str());
                    
                    PRINT(" -> depositstable.modify() : \n");
                    depositstable.modify(*itr, same_payer, [&](auto& a){
                        a.amount -= extended;
                    });
                }
                PRINT("vapaee::book::deposit::aux_substract_deposits() ...\n");
            }

            void aux_add_deposits(name owner, const asset & amount, name ram_payer) {
                PRINT("vapaee::book::deposit::aux_add_deposits()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" amount: ", amount.to_string(), "\n");
                PRINT(" ram_payer: ", ram_payer.to_string(), "\n");

                if (has_auth(owner)) {
                    PRINT(" -> owner has auth : ", owner.to_string(), "\n");
                    // check(owner == ram_payer || contract == ram_payer, create_error_name2(ERROR_AAD_1, owner, ram_payer).c_str());
                }
                if (has_auth(get_self())) {
                    PRINT(" -> contract has auth : ", get_self().to_string(), "\n");
                    // check(owner != ram_payer, create_error_name2(ERROR_AAD_2, contract, ram_payer).c_str());
                }

                if (!vapaee::dex::security::aux_is_token_blacklisted(amount.symbol.code())) {
                    vapaee::dex::tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                    auto tk_itr = tokenstable.find(amount.symbol.code().raw());
                    check(tk_itr != tokenstable.end(), create_error_symbol1(ERROR_AAD_3, amount.symbol).c_str());
                    check(tk_itr->tradeable, create_error_symbol1(ERROR_AAD_4, amount.symbol).c_str());
                }

                asset extended = vapaee::dex::utils::aux_extend_asset(amount);
                PRINT(" extended: ", extended.to_string(), "\n");

                depusers depuserstable(get_self(), get_self().value);
                auto user_itr = depuserstable.find(owner.value);
                if (user_itr == depuserstable.end()) {
                    PRINT(" -> depuserstable.emplace() : \n");
                    // check(has_auth(ram_payer), "ERROR: attempt to allocate RAM without authorization for depusers table");
                    depuserstable.emplace( ram_payer, [&]( auto& a ){
                        a.account = owner;
                    });
                } else {
                    if (ram_payer != get_self()) {
                        // change from using contract RAM to user's RAM 
                        PRINT(" -> depuserstable.modify() ram_payer:",ram_payer.to_string()," \n");
                        depuserstable.modify(*user_itr, ram_payer, [&]( auto& a ){
                            a.account = owner;
                        });
                    }
                }

                deposits depositstable(get_self(), owner.value);
                auto itr = depositstable.find(amount.symbol.code().raw());
                if (itr == depositstable.end()) {
                    
                    // check(has_auth(ram_payer), "ERROR: attempt to allocate RAM without authorization for deposits table");
                    depositstable.emplace( ram_payer, [&]( auto& a ){
                        a.amount = extended;
                    });
                } else {
                    depositstable.modify(*itr, same_payer , [&](auto& a){
                        check(a.amount.symbol == extended.symbol,
                            create_error_asset2(ERROR_AAD_5, a.amount, extended).c_str()); 
                        a.amount += extended;
                    });
                }

                PRINT("vapaee::book::deposit::aux_add_deposits() ...\n");
            }

            void aux_swapdeposit(name from, name to, const asset & quantity, string memo) {
                PRINT("vapaee::book::deposit::aux_swapdeposit()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");

                bool trigger_event = true;
                if (
                    string(TEXT_AGSO_1) == memo ||
                    string(TEXT_ACSO_1) == memo ||
                    string(TEXT_ACSO_2) == memo
                ) {
                    trigger_event = false;
                }
                PRINT(" -> trigger_event: ", std::to_string(trigger_event), "\n");

                check(is_account(to), create_error_name1(ERROR_AS_1, to).c_str());
                auto sym = quantity.symbol.code();
                if (!vapaee::dex::security::aux_is_token_blacklisted(sym)) {
                    vapaee::dex::tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                    const auto& st = tokenstable.get( sym.raw() );
                }

                check(quantity.is_valid(), create_error_asset1(ERROR_AS_2, quantity).c_str());
                check(quantity.amount > 0, create_error_asset1(ERROR_AS_3, quantity).c_str());
                check(memo.size() <= 256, create_error_string1(ERROR_AS_4, memo).c_str());
                
                name ram_payer;
                if (has_auth(to)) {
                    ram_payer = to;
                } else if (has_auth(from)) {
                    ram_payer = from;
                } else {
                    ram_payer = get_self();
                }
                PRINT("   -> ram_payer: ", ram_payer.to_string(), "\n");
                aux_substract_deposits(from, quantity);
                aux_add_deposits(to, quantity, ram_payer);
                
                PRINT("vapaee::book::deposit::aux_swapdeposit() ...\n"); 
            }
            
            void aux_put_deposits_on_user_ram(name owner, const asset & amount) {
                PRINT("vapaee::book::deposit::aux_put_deposits_on_user_ram()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" amount: ", amount.to_string(), "\n");

                if (!has_auth(owner)) {
                    PRINT(" user has no auth -> quitting\n");
                    PRINT("vapaee::book::deposit::aux_put_deposits_on_user_ram()...\n");
                    return;
                }

                depusers depuserstable(get_self(), get_self().value);
                auto user_itr = depuserstable.find(owner.value);
                if (user_itr != depuserstable.end()) {
                    // change from using contract RAM to user's RAM 
                    PRINT(" -> depuserstable.modify() : \n");
                    depuserstable.modify(*user_itr, owner, [&]( auto& a ){
                        a.account = a.account; // dummie operation
                    });
                }

                deposits depositstable(get_self(), owner.value);
                auto itr = depositstable.find(amount.symbol.code().raw());
                if (itr != depositstable.end()) {
                    depositstable.modify(*itr, owner , [&](auto& a){
                        a.amount = a.amount; // dummie operation
                    });
                }

                PRINT("vapaee::book::deposit::aux_put_deposits_on_user_ram() ...\n");
            }

            void aux_transfer_earnings_to_client(name receiver, const asset & quantity, string memo) {
                PRINT("vapaee::book::deposit::aux_transfer_earnings_to_client()\n");

                // get token contract account
                vapaee::dex::tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto tk_ptr = tokenstable.find(quantity.symbol.code().raw());

                deposits depositstable(get_self(), receiver.value);
                auto itr = depositstable.find(quantity.symbol.code().raw());
                if (itr == depositstable.end()) return;

                asset amount = vapaee::dex::utils::aux_get_real_asset(itr->amount);
                
                if (amount.amount <= 100) {
                    PRINT(" -> amount.amount <= 100 -> quitting\n");
                    PRINT("vapaee::book::deposit::aux_transfer_earnings_to_client()...\n");
                    return;
                }

                asset amount_extended = vapaee::dex::utils::aux_extend_asset(amount);
                aux_substract_deposits(receiver, amount_extended);

                action(
                    permission_level{get_self(),name("active")},
                    tk_ptr->contract,
                    name("transfer"),
                    std::make_tuple(get_self(), receiver, amount, memo)
                ).send();
                PRINT("   transfer ", amount.to_string(), " to ", receiver.to_string(),"\n");

                PRINT("vapaee::book::deposit::aux_transfer_earnings_to_client() ...\n");
            } 

            void aux_convert_deposits_to_earnings(const uint64_t ui, asset quantity) {
                PRINT("vapaee::book::deposit::aux_convert_deposits_to_earnings()\n");
                PRINT(" ui: ", std::to_string((long unsigned) ui), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");

                // get interface receiver account and params
                clients uitable(vapaee::dex::contract, vapaee::dex::contract.value);
                auto ptr = uitable.find(ui);
                check(ptr != uitable.end(), create_error_id1(ERROR_ATETC_1, ui).c_str());
                name receiver = ptr->receiver;
                string memo = ptr->params;                

                aux_substract_deposits(get_self(), quantity);
                aux_add_deposits(receiver, quantity, get_self());
                aux_transfer_earnings_to_client(receiver, quantity, memo);

                PRINT("vapaee::book::deposit::aux_convert_deposits_to_earnings() ...\n");
            }

            void aux_earn_micro_change(name owner, symbol orig, symbol extended, name ram_payer, uint64_t client) {
                PRINT("vapaee::book::deposit::aux_earn_micro_change()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" orig: ", orig.code().to_string(), "\n");
                PRINT(" extended: ", extended.code().to_string(), "\n");
                PRINT(" ram_payer: ", ram_payer.to_string(), "\n");

                deposits depositstable(get_self(), owner.value);
                auto itr = depositstable.find(extended.code().raw());
                
                if (itr == depositstable.end()) return;
                // check(itr != depositstable.end(),
                //             create_error_symbol1(ERROR_AEMC_1, extended).c_str());
                PRINT("  before check!\n");
                check(orig.code().raw() == extended.code().raw(),
                            create_error_symbol2(ERROR_AEMC_2, orig, extended).c_str());

                PRINT("  after check!\n");

                asset lowest_real_value = asset(1, orig);
                asset lowest_extended_value = vapaee::dex::utils::aux_extend_asset(lowest_real_value);
                PRINT("   lowest_real_value: ", lowest_real_value.to_string(), "\n");
                PRINT("   lowest_extended_value: ", lowest_extended_value.to_string(), "\n");
                PRINT("   itr->amount: ", itr->amount.to_string(), "\n");
                if (itr->amount < lowest_extended_value) {
                    // transfer to contract fees on CNT
                    vapaee::book::deposit::aux_swapdeposit(owner, get_self(), itr->amount, string(" withdraw micro-change fees: ") + itr->amount.to_string());
                    PRINT("     -- withdraw micro-change fees: ",  itr->amount.to_string(), " from ", owner.to_string(),"\n");
                    // convert deposits to earnings
                    aux_convert_deposits_to_earnings(client, itr->amount);
                    PRINT("     -- converting micro-chang fees ", itr->amount.to_string(), " to earnings\n");
                }

                PRINT("vapaee::book::deposit::aux_earn_micro_change() ...\n");
            }           
            
            void aux_withdraw(name owner, const asset & quantity, uint64_t client) {
                // substract or remove deposit entry
                asset extended = vapaee::dex::utils::aux_extend_asset(quantity);
                aux_substract_deposits(owner, extended);

                aux_earn_micro_change(owner, quantity.symbol, extended.symbol, owner, client);

                name token_contract = vapaee::dex::utils::get_contract_for_token(quantity.symbol.code());

                vapaee::dex::security::aux_check_token_ok(quantity.symbol, token_contract, ERROR_AW_1);

                action(
                    permission_level{get_self(),name("active")},
                    token_contract,
                    name("transfer"),
                    std::make_tuple(get_self(), owner, quantity, string("withdraw: ") + quantity.to_string())
                ).send();
                PRINT("   transfer ", quantity.to_string(), "(",get_self().to_string(),") to ", owner.to_string(),"\n");
            }

            void action_withdraw(name owner, const asset & quantity, uint64_t client) {
                PRINT("vapaee::book::deposit::action_withdraw()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" client: ", std::to_string((long unsigned) client), "\n");

                // if is not an internal inline action then the user "owner" must have beed signed this transaction
                if ( !has_auth( get_self() )) {
                    require_auth( owner );
                }

                aux_withdraw(owner, quantity, client);
                
                PRINT("vapaee::book::deposit::action_withdraw() ...\n");
            }

            bool aux_is_market_being_deleted(uint64_t market_id) {
                PRINT("vapaee::book::deposit::aux_is_market_being_deleted()\n");
                PRINT(" can_market: ", std::to_string((unsigned long) market_id), "\n");
                delmarkets table(get_self(), get_self().value);
                auto ptr = table.find(market_id);
                bool found = ptr != table.end();
                PRINT("vapaee::book::deposit::aux_is_market_being_deleted() ...\n");
                return found;
            }
            
            void handle_book_transfer(name from, name to, asset quantity, string memo, name tokencontract) {
                // skipp handling outcoming transfers from this contract to outside

                asset _quantity;

                if (vapaee::base::global::
                    handler_should_ignore_transfer(from, to, quantity, memo, tokencontract)
                ) {
                    PRINT(from.to_string(), " to ", to.to_string(), ": ", quantity.to_string(), " vapaee::book::deposit::handle_book_transfer() skip...\n");
                    return;
                }
                
                PRINT("vapaee::book::deposit::handle_book_transfer()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");                
                
                vector<string> strings = {""};
                name ram_payer = get_self();
                PRINT(" strings.size(): ", std::to_string(strings.size()), "\n");
                int i,j,s;

                for (i=0,j=0,s=0; i<memo.size(); i++,j++) {
                    if (memo[i] == '|') {
                        s++;
                        j=0;
                        strings.push_back(string(""));
                        continue;
                    } else {
                        strings[s] += memo[i];
                    }
                }
                
                if (strings[0] == string("deposit")) {
                    asset _asset;
                    name receiver;
                    if (strings.size() == 1) {
                        receiver = from;
                    } else if (strings.size() == 2) {
                        receiver = name(strings[1]);
                    }
                    PRINT(" receiver: ", receiver.to_string(), "\n");
                    check(is_account(receiver), "receiver is not a valid account");
                    PRINT(" ram_payer: ", ram_payer.to_string(), "\n");

                    // check if token is valid (token is registered, tradeable, genuine and not blacklisted)
                    vapaee::dex::security::aux_check_token_ok(quantity.symbol, tokencontract, ERROR_HBT_1);

                    _quantity = vapaee::dex::utils::aux_extend_asset(quantity);
                    PRINT(" _quantity extended: ", _quantity.to_string(), "\n");
                    aux_add_deposits(receiver, _quantity, get_self());
                }

                PRINT("vapaee::book::deposit::handle_book_transfer() ...\n");
            }
        };     
    };
};
