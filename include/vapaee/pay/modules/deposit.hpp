#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pay/tables.hpp>
#include <vapaee/pay/errors.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <vapaee/dex/modules/utils.hpp>
// #include <vapaee/pay/modules/rex.hpp>


namespace vapaee {
    namespace pay {
        namespace deposit {

            inline name get_self() {
                return vapaee::pay::contract;
            }

            void aux_substract_deposits(name owner, const asset & amount) {
                PRINT("vapaee::pay::deposit::aux_substract_deposits()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" amount: ", amount.to_string(), "\n");

                deposits depositstable(vapaee::pay::contract, owner.value);
                auto itr = depositstable.find(amount.symbol.code().raw());
                check(itr != depositstable.end(),
                            create_error_asset1(ERROR_APSD_1, amount).c_str());

                //check(itr->amount.symbol == amount.symbol,
                //    create_error_asset2(ERROR_APSD_2, itr->amount, amount).c_str());
                asset extended = vapaee::dex::utils::aux_extend_asset(amount);
                PRINT(" extended: ", extended.to_string(), "\n");
                if (itr->amount == extended) {
                    PRINT("  (itr->amount == amount): (",  amount.to_string(), ")\n");
                    PRINT(" -> depositstable.erase() : \n");
                    depositstable.erase(itr);
                } else {
                    PRINT("  (itr->amount > amount): (", itr->amount.to_string(), " > ", amount.to_string(),  ")\n");
                    check(itr->amount > extended,
                            create_error_asset2(ERROR_APSD_3, extended, itr->amount).c_str());
                    
                    PRINT(" -> depositstable.modify() : \n");
                    depositstable.modify(*itr, same_payer, [&](auto& a){
                        a.amount -= extended;
                    });
                }
                PRINT("vapaee::pay::deposit::aux_substract_deposits() ...\n");
            }

            void aux_add_deposits(name owner, const asset & amount, name ram_payer) {
                PRINT("vapaee::pay::deposit::aux_add_deposits()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" amount: ", amount.to_string(), "\n");
                PRINT(" ram_payer: ", ram_payer.to_string(), "\n");

                if (has_auth(owner)) {
                    PRINT(" -> owner has auth : ", owner.to_string(), "\n");
                    // check(owner == ram_payer || vapaee::pay::contract == ram_payer, create_error_name2(ERROR_APAD_1, owner, ram_payer).c_str());
                }
                if (has_auth(vapaee::pay::contract)) {
                    PRINT(" -> contract has auth : ", vapaee::pay::contract.to_string(), "\n");
                    // check(owner != ram_payer, create_error_name2(ERROR_APAD_2, contract, ram_payer).c_str());
                }

                if (!vapaee::dex::security::aux_is_token_blacklisted(amount.symbol.code())) {
                    tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                    auto tk_itr = tokenstable.find(amount.symbol.code().raw());
                    check(tk_itr != tokenstable.end(), create_error_symbol1(ERROR_APAD_3, amount.symbol).c_str());
                    check(tk_itr->tradeable, create_error_symbol1(ERROR_APAD_4, amount.symbol).c_str());
                }

                asset extended = vapaee::dex::utils::aux_extend_asset(amount);
                PRINT(" extended: ", extended.to_string(), "\n");

                depusers depuserstable(vapaee::pay::contract, vapaee::pay::contract.value);
                auto user_itr = depuserstable.find(owner.value);
                if (user_itr == depuserstable.end()) {
                    PRINT(" -> depuserstable.emplace() : \n");
                    // check(has_auth(ram_payer), "ERROR: attempt to allocate RAM without authorization for depusers table");
                    depuserstable.emplace( ram_payer, [&]( auto& a ){
                        a.account = owner;
                    });
                } else {
                    if (ram_payer != vapaee::pay::contract) {
                        // change from using contract RAM to user's RAM 
                        PRINT(" -> depuserstable.modify() ram_payer:",ram_payer.to_string()," \n");
                        depuserstable.modify(*user_itr, ram_payer, [&]( auto& a ){
                            a.account = owner;
                        });
                    }
                }

                deposits depositstable(vapaee::pay::contract, owner.value);
                auto itr = depositstable.find(amount.symbol.code().raw());
                if (itr == depositstable.end()) {
                    
                    // check(has_auth(ram_payer), "ERROR: attempt to allocate RAM without authorization for deposits table");
                    depositstable.emplace( ram_payer, [&]( auto& a ){
                        a.amount = extended;
                    });
                } else {
                    depositstable.modify(*itr, same_payer , [&](auto& a){
                        check(a.amount.symbol == extended.symbol,
                            create_error_asset2(ERROR_APAD_5, a.amount, extended).c_str()); 
                        a.amount += extended;
                    });
                }

                PRINT("vapaee::pay::deposit::aux_add_deposits() ...\n");
            }

            void aux_swapdeposit(name from, name to, const asset & quantity, string memo) {
                PRINT("vapaee::pay::deposit::aux_swapdeposit()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");

                check(is_account(to), create_error_name1(ERROR_APS_1, to).c_str());
                auto sym = quantity.symbol.code();
                if (!vapaee::dex::security::aux_is_token_blacklisted(sym)) {
                    tokens tokenstable(vapaee::dex::contract, vapaee::dex::contract.value);
                    const auto& st = tokenstable.get( sym.raw() );
                }

                check(quantity.is_valid(), create_error_asset1(ERROR_APS_2, quantity).c_str());
                check(quantity.amount > 0, create_error_asset1(ERROR_APS_3, quantity).c_str());
                check(memo.size() <= 256, create_error_string1(ERROR_APS_4, memo).c_str());
                
                name ram_payer;
                if (has_auth(to))
                    ram_payer = to;

                else if (has_auth(from))
                    ram_payer = from;

                else
                    ram_payer = vapaee::pay::contract;
                
                PRINT("   -> ram_payer: ", ram_payer.to_string(), "\n");
                aux_substract_deposits(from, quantity);
                aux_add_deposits(to, quantity, ram_payer);
                
                PRINT("vapaee::pay::deposit::aux_swapdeposit() ...\n"); 
            }

            void aux_put_deposits_on_user_ram(name owner, const asset & amount) {
                PRINT("vapaee::pay::deposit::aux_put_deposits_on_user_ram()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" amount: ", amount.to_string(), "\n");

                if (!has_auth(owner)) {
                    PRINT(" user has no auth -> quitting\n");
                    PRINT("vapaee::pay::deposit::aux_put_deposits_on_user_ram()...\n");
                    return;
                }

                depusers depuserstable(vapaee::pay::contract, vapaee::pay::contract.value);
                auto user_itr = depuserstable.find(owner.value);
                if (user_itr != depuserstable.end()) {
                    // change from using contract RAM to user's RAM 
                    PRINT(" -> depuserstable.modify() : \n");
                    depuserstable.modify(*user_itr, owner, [&]( auto& a ){
                        a.account = a.account; // dummie operation
                    });
                }

                deposits depositstable(vapaee::pay::contract, owner.value);
                auto itr = depositstable.find(amount.symbol.code().raw());
                if (itr != depositstable.end()) {
                    depositstable.modify(*itr, owner , [&](auto& a){
                        a.amount = a.amount; // dummie operation
                    });
                }

                PRINT("vapaee::pay::deposit::aux_put_deposits_on_user_ram() ...\n");
            }   

            void perform_deposit(name receiver, const asset & quantity) {
                PRINT("vapaee::pay::deposit::perform_deposit()\n");
                aux_add_deposits(receiver, quantity, get_self());
            }
            
            void perform_withdraw(name owner, const asset & quantity) {

                // substract or remove deposit entry
                asset extended = vapaee::dex::utils::aux_extend_asset(quantity);
                aux_substract_deposits(owner, extended);

                name token_contract = vapaee::dex::utils::get_contract_for_token(quantity.symbol.code());

                vapaee::dex::security::aux_check_token_ok(quantity.symbol, token_contract, ERROR_PW_1);

                action(
                    permission_level{vapaee::pay::contract,name("active")},
                    token_contract,
                    name("transfer"),
                    std::make_tuple(vapaee::pay::contract, owner, quantity, string("withdraw: ") + quantity.to_string())
                ).send();
                PRINT("   transfer ", quantity.to_string(), "(",vapaee::pay::contract.to_string(),") to ", owner.to_string(),"\n");
            }

            void action_withdraw(name owner, const asset & quantity) {
                PRINT("vapaee::pay::deposit::action_withdraw()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");

                // if is not an internal inline action then the user "owner" must have beed signed this transaction
                if ( !has_auth( vapaee::pay::contract )) {
                    require_auth( owner );
                }

                perform_withdraw(owner, quantity);
                
                PRINT("vapaee::pay::deposit::action_withdraw() ...\n");
            }
        };     
    };
};
