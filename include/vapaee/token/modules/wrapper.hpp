#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/token/errors.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <vapaee/dex/modules/token.hpp>
#include <vapaee/token/modules/utils.hpp>

namespace vapaee {
    namespace token {
        namespace standard {
            asset get_supply( const name& token_contract_account, const symbol_code& sym_code, const string& error );
        }
    };
    namespace dex {
        namespace token {
            asset get_token_supply(const symbol_code& token);
        };
    };
};



namespace vapaee {
    namespace token {
        namespace wrapper {

            inline name get_self() {
                return vapaee::token::contract;
            }

            // ------------------------------
            const name _no_account_ = name(".");
            
            name get_token_foreign_contract(const symbol_code& token) {
                PRINT("vapaee::token::wrapper::get_token_foreign_contract()\n");
                vapaee::token::tokens tokens_table(get_self(), get_self().value);
                auto ptr = tokens_table.find(token.raw());
                if (ptr != tokens_table.end()) {
                    return ptr->account;
                } else {
                    return _no_account_;
                }
            }

            bool is_token_registered_as_foreign(const symbol_code& token) {
                PRINT("vapaee::token::wrapper::is_token_registered_as_foreign()\n");
                return get_token_foreign_contract(token) != vapaee::token::contract;
            }

            asset get_token_foreign_supply(const symbol_code& token) {
                PRINT("vapaee::token::wrapper::get_token_foreign_supply()\n");
                name contract = get_token_foreign_contract(token);
                if (contract == _no_account_) {
                    return asset(0, symbol(token, 0));
                } else {
                    return vapaee::token::standard::get_supply(contract, token, 
                        create_error_symcode1("ERR-GTFS-01: can't get supply of foreign token", token).c_str());
                }
            }

            asset get_token_supply(const symbol_code& token, const string& error) {
                PRINT("vapaee::token::wrapper::get_token_supply()\n");
                // TODO: rewrite this in a more elegant way
                name contract = get_token_foreign_contract(token);
                PRINT(" > contract: ", contract.to_string(), "\n");

                if (contract == _no_account_) {
                    // not foreign or not regisered.
                    stats stat_table( get_self(), token.raw() );
                    auto ptr = stat_table.find(token.raw());
                    if (ptr != stat_table.end()) {
                        // is vapaeetokens native token.
                        return ptr->supply;
                    } else {
                        // try to find it in vapaee::dex::contrat
                        bool found = false;
                        asset supply = vapaee::dex::token::get_token_supply(token);
                        if (supply.amount != 0 || supply.symbol.precision() != 0) {
                            found = true;
                        }
                        check(found, error + " Reason: token is not vapaeetokens native nor registered as foreign. Consider to deposit and then withdraw any amount or this token in vapaeetokens and try again.");
                        return supply;
                    }
                } else {
                    return get_token_foreign_supply(token);
                }
                return asset(0, symbol(token, 0));
            }

            // ------------------------------


            void action_deposit(const name& owner, const asset& quantity, const name& token_contract, const string& back_memo) {
                require_auth(get_self());
                symbol_code symcode = quantity.symbol.code();

                check(token_contract != get_self(), "You can't deposit this token because it is already under vapaeetokens countract");

                // solo aceptar tokens registrados en telosmaindex
                vapaee::dex::security::aux_check_token_ok(quantity.symbol, token_contract, "ERROR_AD_1: can't deposit unknown assets. Please register the token in telosmaindex first, then try again.");

                // assert token registration
                vapaee::token::utils::assert_token_registration(name("add"), quantity, token_contract);

                string memo = string("issuing ") +  quantity.to_string() + " for " + owner.to_string() + " deposited from " + token_contract.to_string();
                memo = string("Issue ") + quantity.to_string() + " because of a deposit from " + owner.to_string();
                vapaee::token::utils::send_issue_tokens(quantity, memo.c_str(), vapaee::token::contract);

                memo = string("You deposited ") + quantity.to_string() + " into vapaeetokens. Now you can bennefit from all Vapaée token services we provide in this contract. This services is going to be free for limited time.";
                if (back_memo.size() > 0) {
                    memo = back_memo;
                }
                vapaee::token::utils::send_transfer_tokens(get_self(), owner, quantity, memo.c_str(), vapaee::token::contract);
            }

            void action_withdraw(const name& owner, const asset& quantity, const string& notes) {
                PRINT("vapaee::token::wrapper::action_withdraw()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                symbol_code symcode = quantity.symbol.code();

                require_auth(owner);

                // verificamos si existe el token as foreign.
                vapaee::token::tokens tokens_table(get_self(), get_self().value);
                auto ptr = tokens_table.find(symcode.raw());
                check(ptr != tokens_table.end(), create_error_asset1(
                    "ERROR_AW_1: The tokens does not exist in vapaeetokens tokens table.", quantity).c_str() );
                check(ptr->account != vapaee::token::contract, "ERROR_AW_2: You can't withdraw a native token. This asset is not marked as foreign.");

                check(quantity.symbol.precision() == ptr->supply.symbol.precision(),
                    create_error_asset2("ERROR_AW_3: The symbol of the quantity withdrawn differs from the symbol taken from the token contract max_supply property",
                    quantity,
                    ptr->supply).c_str()
                );

                name token_contract = ptr->account;

                // -- send transfer quantity from owner to get_self() --> vapaeetokens (hay que forzar el transfer)
                string memo = string("Debit ") + quantity.to_string() + " fom " + owner.to_string() + " for withdrawal";
                vapaee::token::utils::send_transfer_tokens(owner, get_self(), quantity, memo, vapaee::token::contract);

                // -- send burn (quantity) --> vapaeetokens 
                memo = string("Burn ") + quantity.to_string() + " because of a withdrawal from " + owner.to_string();
                vapaee::token::utils::send_burn_tokens(quantity, memo, vapaee::token::contract);

                // -- send transfer quantity from get_self() to owner --> token_contract
                memo = string("You withdrew ") + quantity.to_string() + " from vapaeetokens. Good luck!";
                if (notes.size() > 0) {
                    // TODO: notes may be more complex to adjust not only the final memo
                    memo = notes;
                }
                vapaee::token::utils::send_transfer_tokens(get_self(), owner, quantity, memo, token_contract);

                // assert token registration
                vapaee::token::utils::assert_token_registration(name("sub"), quantity, token_contract);
                
                PRINT("vapaee::token::wrapper::action_withdraw()...\n");
            }

        };     
    };
};


#include <vapaee/token/modules/standard.hpp>