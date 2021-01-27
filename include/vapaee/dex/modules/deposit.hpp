#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/dex/constants.hpp>
#include <vapaee/dex/errors.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/dex/modules/utils.hpp>
#include <vapaee/dex/modules/record.hpp>
#include <vapaee/dex/modules/market.hpp>
#include <vapaee/dex/modules/client.hpp>

namespace vapaee {
    namespace dex {

        using namespace error;
        using namespace utils;
        using namespace record;

        namespace deposit {

            void aux_substract_deposits(name owner, const asset & amount) {
                PRINT("vapaee::dex::deposit::aux_substract_deposits()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" amount: ", amount.to_string(), "\n");

                deposits depositstable(contract, owner.value);
                auto itr = depositstable.find(amount.symbol.code().raw());
                check(itr != depositstable.end(),
                            create_error_asset1(ERROR_ASD_1, amount).c_str());

                check(itr->amount.symbol == amount.symbol,
                    create_error_asset2(ERROR_ASD_2, itr->amount, amount).c_str());
                if (itr->amount == amount) {
                    PRINT("  (itr->amount == amount): (",  amount.to_string(), ")\n");
                    PRINT(" -> depositstable.erase() : \n");
                    depositstable.erase(itr);
                } else {
                    PRINT("  (itr->amount > amount): (", itr->amount.to_string(), " > ", amount.to_string(),  ")\n");
                    check(itr->amount > amount,
                            create_error_asset2(ERROR_ASD_3, amount, itr->amount).c_str());
                    
                    PRINT(" -> depositstable.modify() : \n");
                    depositstable.modify(*itr, same_payer, [&](auto& a){
                        a.amount -= amount;
                    });
                }
                PRINT("vapaee::dex::deposit::aux_substract_deposits() ...\n");
            }

            bool aux_is_token_blacklisted(const symbol_code &sym_code) {
                PRINT("vapaee::dex::deposit::aux_is_token_blacklisted()\n");
                PRINT(" sym_code: ", sym_code.to_string(), "\n");
                
                tokens token_table(contract, contract.value);
                auto ptr = token_table.find(sym_code.raw());
                if (ptr != token_table.end()) {
                    PRINT("vapaee::dex::deposit::aux_is_token_blacklisted() ... -> false\n");
                    return false;
                }

                blacklist list(contract, contract.value); 
                auto index = list.get_index<name("symbol")>();
                auto itr = index.lower_bound(sym_code.raw());
                for (auto itr = index.lower_bound(sym_code.raw()); itr != index.end(); itr++) {
                    if (itr->symbol == sym_code) {
                        PRINT("vapaee::dex::deposit::aux_is_token_blacklisted() ... -> true\n");
                        return true;
                    } else {
                        break;
                    }
                }
                PRINT("vapaee::dex::deposit::aux_is_token_blacklisted() ... -> false\n");
                return false;
            }

            void aux_add_deposits(name owner, const asset & amount, name ram_payer) {
                PRINT("vapaee::dex::deposit::aux_add_deposits()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" amount: ", amount.to_string(), "\n");
                PRINT(" ram_payer: ", ram_payer.to_string(), "\n");

                if (has_auth(owner)) {
                    PRINT(" -> owner has auth : ", owner.to_string(), "\n");
                    // check(owner == ram_payer || contract == ram_payer, create_error_name2(ERROR_AAD_1, owner, ram_payer).c_str());
                }
                if (has_auth(contract)) {
                    PRINT(" -> contract has auth : ", contract.to_string(), "\n");
                    // check(owner != ram_payer, create_error_name2(ERROR_AAD_2, contract, ram_payer).c_str());
                }

                if (!aux_is_token_blacklisted(amount.symbol.code())) {
                    tokens tokenstable(contract, contract.value);
                    auto tk_itr = tokenstable.find(amount.symbol.code().raw());
                    check(tk_itr != tokenstable.end(), create_error_symbol1(ERROR_AAD_3, amount.symbol).c_str());
                    check(tk_itr->tradeable, create_error_symbol1(ERROR_AAD_4, amount.symbol).c_str());
                }

                depusers depuserstable(contract, contract.value);
                auto user_itr = depuserstable.find(owner.value);
                if (user_itr == depuserstable.end()) {
                    PRINT(" -> depuserstable.emplace() : \n");
                    // check(has_auth(ram_payer), "ERROR: attempt to allocate RAM without authorization for depusers table");
                    depuserstable.emplace( ram_payer, [&]( auto& a ){
                        a.account = owner;
                    });
                } else {
                    if (ram_payer != contract) {
                        // change from using contract RAM to user's RAM 
                        PRINT(" -> depuserstable.modify() ram_payer:",ram_payer.to_string()," \n");
                        depuserstable.modify(*user_itr, ram_payer, [&]( auto& a ){
                            a.account = owner;
                        });
                    }
                }

                deposits depositstable(contract, owner.value);
                auto itr = depositstable.find(amount.symbol.code().raw());
                if (itr == depositstable.end()) {
                    
                    // check(has_auth(ram_payer), "ERROR: attempt to allocate RAM without authorization for deposits table");
                    depositstable.emplace( ram_payer, [&]( auto& a ){
                        a.amount = amount;
                    });
                } else {
                    depositstable.modify(*itr, same_payer , [&](auto& a){
                        check(a.amount.symbol == amount.symbol,
                            create_error_asset2(ERROR_AAD_5, a.amount, amount).c_str()); 
                        a.amount += amount;
                    });
                }

                PRINT("vapaee::dex::deposit::aux_add_deposits() ...\n");
            }
            
            void aux_put_deposits_on_user_ram(name owner, const asset & amount) {
                PRINT("vapaee::dex::deposit::aux_put_deposits_on_user_ram()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" amount: ", amount.to_string(), "\n");

                if (!has_auth(owner)) {
                    PRINT(" user has no auth -> quitting\n");
                    PRINT("vapaee::dex::deposit::aux_put_deposits_on_user_ram()...\n");
                    return;
                }

                depusers depuserstable(contract, contract.value);
                auto user_itr = depuserstable.find(owner.value);
                if (user_itr != depuserstable.end()) {
                    // change from using contract RAM to user's RAM 
                    PRINT(" -> depuserstable.modify() : \n");
                    depuserstable.modify(*user_itr, owner, [&]( auto& a ){
                        a.account = a.account; // dummie operation
                    });
                }

                deposits depositstable(contract, owner.value);
                auto itr = depositstable.find(amount.symbol.code().raw());
                if (itr != depositstable.end()) {
                    depositstable.modify(*itr, owner , [&](auto& a){
                        a.amount = a.amount; // dummie operation
                    });
                }

                PRINT("vapaee::dex::deposit::aux_put_deposits_on_user_ram() ...\n");
            }

            void aux_earn_micro_change(name owner, symbol orig, symbol extended, name ram_payer, uint64_t client) {
                PRINT("vapaee::dex::deposit::aux_earn_micro_change()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" orig: ", orig.code().to_string(), "\n");
                PRINT(" extended: ", extended.code().to_string(), "\n");
                PRINT(" ram_payer: ", ram_payer.to_string(), "\n");

                deposits depositstable(contract, owner.value);
                auto itr = depositstable.find(extended.code().raw());
                
                if (itr == depositstable.end()) return;
                // check(itr != depositstable.end(),
                //             create_error_symbol1(ERROR_AEMC_1, extended).c_str());
                PRINT("  before check!\n");
                check(orig.code().raw() == extended.code().raw(),
                            create_error_symbol2(ERROR_AEMC_2, orig, extended).c_str());

                PRINT("  after check!\n");

                asset lowest_real_value = asset(1, orig);
                asset lowest_extended_value = aux_extend_asset(lowest_real_value);
                PRINT("   lowest_real_value: ", lowest_real_value.to_string(), "\n");
                PRINT("   lowest_extended_value: ", lowest_extended_value.to_string(), "\n");
                PRINT("   itr->amount: ", itr->amount.to_string(), "\n");
                if (itr->amount < lowest_extended_value) {
                    // transfer to contract fees on CNT
                    action(
                        permission_level{contract,name("active")},
                        contract,
                        name("swapdeposit"),
                        std::make_tuple(owner, contract, itr->amount, string(" withdraw micro-change fees: ") + itr->amount.to_string())
                    ).send();

                    PRINT("     -- withdraw micro-change fees: ",  itr->amount.to_string(), " from ", owner.to_string(),"\n");
                    // convert deposits to earnings
                    action(
                        permission_level{contract,name("active")},
                        contract,
                        name("deps2earn"),
                        std::make_tuple(client, itr->amount)
                    ).send();
                    PRINT("     -- converting micro-chang fees ", itr->amount.to_string(), " to earnings\n");
                }

                PRINT("vapaee::dex::deposit::aux_earn_micro_change() ...\n");
            }

            void aux_transfer_earnings_to_client(const uint64_t ui, const asset & quantity) {
                PRINT("vapaee::dex::deposit::aux_transfer_earnings_to_client()\n");
                PRINT(" ui: ", std::to_string((long unsigned) ui), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");

                // get interface receiver account and params
                clients uitable(contract, contract.value);
                auto ptr = uitable.find(ui);
                check(ptr != uitable.end(), create_error_id1(ERROR_ATETC_1, ui).c_str());
                name receiver = ptr->receiver;
                string memo = ptr->params;

                // get token contract account
                tokens tokenstable(contract, contract.value);
                auto tk_ptr = tokenstable.find(quantity.symbol.code().raw());

                asset amount = aux_get_real_asset(quantity);

                action(
                    permission_level{contract,name("active")},
                    tk_ptr->contract,
                    name("transfer"),
                    std::make_tuple(contract, receiver, amount, memo)
                ).send();
                PRINT("   transfer ", amount.to_string(), " to ", receiver.to_string(),"\n");

                PRINT("vapaee::dex::deposit::aux_transfer_earnings_to_client() ...\n");
            }            

            void aux_convert_deposits_to_earnings(const uint64_t ui, asset quantity) {
                PRINT("vapaee::dex::deposit::aux_convert_deposits_to_earnings()\n");
                PRINT(" ui: ", std::to_string((long unsigned) ui), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");

                aux_substract_deposits(contract, quantity);
                aux_transfer_earnings_to_client(ui, quantity);

                PRINT("vapaee::dex::deposit::aux_convert_deposits_to_earnings() ...\n");
            }            

            void action_withdraw(name owner, const asset & quantity, uint64_t client) {
                PRINT("vapaee::dex::deposit::action_withdraw()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" client: ", std::to_string((long unsigned) client), "\n");

                // if is not an internal inline action then the user "owner" must have beed signed this transaction
                if ( !has_auth( contract )) {
                    require_auth( owner );
                }

                // substract or remove deposit entry
                asset extended = aux_extend_asset(quantity);
                aux_substract_deposits(owner, extended);

                aux_earn_micro_change(owner, quantity.symbol, extended.symbol, owner, client);

                // send tokens
                name contract = name(0);
                tokens tokenstable(contract, contract.value);
                auto ptk_itr = tokenstable.find(quantity.symbol.code().raw());
                
                if (ptk_itr != tokenstable.end()) {
                    contract = ptk_itr->contract;
                } else {
                    // try and see if the token is_blacklisted
                    blacklist list(contract, contract.value); 
                    auto index = list.get_index<name("symbol")>();                    
                    for (auto itr = index.lower_bound(quantity.symbol.code().raw()); itr != index.end(); itr++) {
                        if (itr->symbol == quantity.symbol.code()) {
                            contract = itr->contract;
                        }
                    }
                }

                check(contract != name(0), create_error_symcode1(ERROR_AW_1, quantity.symbol.code()).c_str());

                action(
                    permission_level{contract,name("active")},
                    contract,
                    name("transfer"),
                    std::make_tuple(contract, owner, quantity, string("withdraw: ") + quantity.to_string())
                ).send();
                PRINT("   transfer ", quantity.to_string(), "(",contract.to_string(),") to ", owner.to_string(),"\n");

                aux_register_event(owner, name("withdraw"), quantity.to_string());

                asset _asset;
                aux_trigger_event(quantity.symbol.code(), name("withdraw"), owner, contract, quantity, _asset, _asset);

                PRINT("vapaee::dex::deposit::action_withdraw() ...\n");
            }

            bool aux_is_market_being_deleted(uint64_t market_id) {
                PRINT("vapaee::dex::deposit::aux_is_market_being_deleted()\n");
                PRINT(" can_market: ", std::to_string((unsigned long) market_id), "\n");
                delmarkets table(contract, contract.value);
                auto ptr = table.find(market_id);
                bool found = ptr != table.end();
                PRINT("vapaee::dex::deposit::aux_is_market_being_deleted() ...\n");
                return found;
            }

            void action_swapdeposit(name from, name to, const asset & quantity, string memo) {
                PRINT("vapaee::dex::deposit::action_swapdeposit()\n");
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


                check( from != to, "cannot swap deposits to self" );

                // if is not an internal inline action then the user "from" must have beed signed this transaction
                if ( !has_auth( contract )) {
                    require_auth( from );
                }
                
                check( is_account( to ), "to account does not exist");
                auto sym = quantity.symbol.code();
                if (!aux_is_token_blacklisted(sym)) {
                    tokens tokenstable(contract, contract.value);
                    const auto& st = tokenstable.get( sym.raw() );
                }

                require_recipient( from );
                require_recipient( to );

                check( quantity.is_valid(), "invalid quantity" );
                check( quantity.amount > 0, "must transfer positive quantity" );
                check( quantity.symbol.precision() == internal_precision, "symbol precision mismatch" );
                check( memo.size() <= 256, "memo has more than 256 bytes" );
                
                name ram_payer;
                if ( has_auth( to ) ) {
                    ram_payer = to;
                } else if (has_auth( from )) {
                    ram_payer = from;
                } else {
                    ram_payer = contract;
                }
                PRINT("   -> ram_payer: ", ram_payer.to_string(), "\n");
                aux_substract_deposits(from, quantity);
                aux_add_deposits(to, quantity, ram_payer);

                if (from != contract && to != contract) {
                    trigger_event = true;
                    PRINT(" -> trigger_event: ", std::to_string(trigger_event), "\n");
                }

                if (trigger_event) {
                    asset _asset;
                    aux_trigger_event(quantity.symbol.code(), name("swapdeposit"), from, to, quantity, _asset, _asset);
                }               
                
                PRINT("vapaee::dex::deposit::action_swapdeposit() ...\n"); 
            }
            
            void action_convert_deposits_to_earnings(const uint64_t client, const asset & quantity) {
                PRINT("vapaee::dex::deposit::action_convert_deposits_to_earnings()\n");
                PRINT(" client: ", std::to_string((long unsigned) client), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                require_auth(contract);

                aux_convert_deposits_to_earnings(client, quantity);
                PRINT("vapaee::dex::deposit::action_convert_deposits_to_earnings() ...\n");
            }

            void handler_transfer(name from, name to, asset quantity, string memo) {
                // skipp handling outcoming transfers from this contract to outside
                asset _quantity;
                if (to != contract) {
                    print(from.to_string(), " to ", to.to_string(), ": ", quantity.to_string(), " vapaee::dex::deposit::handler_transfer() skip...\n");
                    return;
                }
                
                PRINT("vapaee::dex::deposit::handler_transfer()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" to: ", to.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");                
                
                vector<string> strings = {""};
                name ram_payer = contract;
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
                    _quantity = aux_extend_asset(quantity);
                    PRINT(" _quantity extended: ", _quantity.to_string(), "\n");
                    aux_add_deposits(receiver, _quantity, contract);
                    aux_register_event(from, name("deposit"), receiver.to_string() + "|" + _quantity.to_string());
                    aux_trigger_event(_quantity.symbol.code(), name("deposit"), from, receiver, _quantity, _asset, _asset);
                }

                PRINT("vapaee::dex::deposit::handler_transfer() ...\n");
            }                 
        };     
    };
};
