#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/token/errors.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <vapaee/dex/modules/token.hpp>

namespace vapaee {
    namespace token {
        namespace wrapper {

            inline name get_self() {
                return vapaee::token::contract;
            }

            void send_create_token(const asset& max_supply) {
                action(
                    permission_level{get_self(), "active"_n},
                    get_self(),
                    "create"_n,
                    make_tuple(
                        get_self(),
                        max_supply
                    )
                ).send();
            }

            void send_issue_tokens(const asset& quantity, const string& memo ) {
                action(
                    permission_level{get_self(), "active"_n},
                    get_self(),
                    "issue"_n,
                    make_tuple(
                        get_self(),
                        quantity,
                        memo
                    )
                ).send();
            }

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

            void send_burn_tokens(const asset& quantity, const string& memo) {
                action(
                    permission_level{get_self(), "active"_n},
                    get_self(),
                    "retire"_n,
                    make_tuple(
                        quantity,
                        memo
                    )
                ).send();
            }

            // ------------------------------
            const name _no_account_ = name(".");
            
            name get_token_foreign_contract(const symbol_code& token) {
                PRINT("vapaee::token::wrapper::get_token_foreign_contract()\n");
                // verificamos si existe el token as foreign. Si no, lo creamos
                foreign foreign_table(get_self(), get_self().value);
                auto ptr = foreign_table.find(token.raw());
                if (ptr != foreign_table.end()) {
                    return ptr->account;
                } else {
                    return _no_account_;
                }
            }

            bool is_token_registered_as_foreign(const symbol_code& token) {
                PRINT("vapaee::token::wrapper::is_token_registered_as_foreign()\n");
                return get_token_foreign_contract(token) != _no_account_;
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
                        check(found, error + " Reason: token is not vapaeetokens native nor registered as foreign.");
                    }
                } else {
                    return get_token_foreign_supply(token);
                }
                return asset(0, symbol(token, 0));
            }

            // ------------------------------


            void action_deposit(const name& owner, const asset& quantity, const name& token_contract) {
                require_auth(get_self());
                symbol_code symcode = quantity.symbol.code();


                check(token_contract != get_self(), "You can't deposit this token because it is already under vapaeetokens countract");

                // verificamos si existe el token as foreign. Si no, lo creamos
                foreign foreign_table(get_self(), get_self().value);
                auto ptr = foreign_table.find(symcode.raw());
                if (ptr == foreign_table.end()) {
                    foreign_table.emplace(get_self(), [&]( auto& a ){
                        a.symcode = symcode;
                        a.account = token_contract;
                    });

                    stats origin_stats_table( token_contract, symcode.raw() );
                    auto origin_token = origin_stats_table.find( symcode.raw() );

                    check(quantity.symbol == origin_token->max_supply.symbol,
                        create_error_asset2("Inconsistency found: The symbol of the quantity deposited differs from the symbol taken from the token contract max_supply property", quantity, origin_token->max_supply).c_str() );

                    send_create_token(origin_token->max_supply);
                }

                string memo = string("issuing ") +  quantity.to_string() + " for " + owner.to_string() + " deposited from " + token_contract.to_string();
                memo = string("Issue ") + quantity.to_string() + " because of a deposit from " + owner.to_string();
                send_issue_tokens(quantity, memo.c_str());

                memo = string("You deposited ") + quantity.to_string() + " into vapaeetokens. Now you can bennefit from all Vapaée token services we provide in this contract. This services is going to be free for limited time.";
                send_transfer_tokens(get_self(), owner, quantity, memo.c_str(), get_self());

                // solo aceptar tokens registrados en telosmaindex
                vapaee::dex::security::aux_check_token_ok(quantity.symbol, token_contract, "ERROR_AD_1: can't deposit unknown assets. Please register the token in telosmaindex first, then try again.");

            }

            void action_withdraw(const name& owner, const asset& quantity, const string& notes) {
                PRINT("vapaee::token::wrapper::action_withdraw()\n");
                PRINT(" owner: ", owner.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                symbol_code symcode = quantity.symbol.code();

                require_auth(owner);

                // verificamos si existe el token as foreign.
                foreign foreign_table(get_self(), get_self().value);
                auto ptr = foreign_table.find(symcode.raw());
                check(ptr != foreign_table.end(), "ERROR_AW_1: You can't withdraw a native token. This asset is not marked as foreign.");

                name token_contract = ptr->account;

                // -- send transfer quantity from owner to get_self() --> vapaeetokens (hay que forzar el transfer)
                string memo = string("Debit ") + quantity.to_string() + " fom " + owner.to_string() + " for withdrawal";
                send_transfer_tokens(owner, get_self(), quantity, memo, get_self());

                // -- send burn (quantity) --> vapaeetokens 
                memo = string("Burn ") + quantity.to_string() + " because of a withdrawal from " + owner.to_string();
                send_burn_tokens(quantity, memo);

                // -- send transfer quantity from get_self() to owner --> token_contract
                memo = string("You withdrew ") + quantity.to_string() + " from vapaeetokens. Good luck!";
                if (notes.size() > 0) {
                    // TODO: notes may be more complex to adjust not only the final memo
                    memo = notes;
                }
                send_transfer_tokens(get_self(), owner, quantity, memo, token_contract);
                
                PRINT("vapaee::token::wrapper::action_withdraw()...\n");
            }

        };     
    };
};
