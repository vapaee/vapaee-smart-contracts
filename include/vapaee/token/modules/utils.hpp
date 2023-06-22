#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/token/tables.hpp>
#include <vapaee/dex/modules/token.hpp>

namespace vapaee {
    namespace token {
        namespace utils {

            inline name get_self() {
                return vapaee::token::contract;
            }

            void send_burn_tokens(const asset& quantity, const string& memo, const name& contract) {
                PRINT("vapaee::token::utils::send_burn_tokens()\n");
                action(
                    permission_level{vapaee::current_contract, "active"_n},
                    contract,
                    "retire"_n,
                    make_tuple(
                        quantity,
                        memo
                    )
                ).send();
            }

            void send_burn_tokens(const asset& quantity, const string& memo) {
                PRINT("vapaee::token::utils::send_burn_tokens()\n");
                send_burn_tokens(quantity, memo, get_self());
            }



            void send_create_token(const asset& max_supply, const name& contract) {
                PRINT("vapaee::token::utils::send_create_token()\n");
                PRINT(" max_supply: ", max_supply.to_string(), "\n");
                PRINT(" contract: ", contract.to_string(), "\n");
                action(
                    permission_level{vapaee::current_contract, "active"_n},
                    contract,
                    "create"_n,
                    make_tuple(
                        vapaee::current_contract,
                        max_supply
                    )
                ).send();
            }

            void send_create_token(const asset& max_supply) {
                PRINT("vapaee::token::utils::send_create_token()\n");
                PRINT(" max_supply: ", max_supply.to_string(), "\n");
                send_create_token(max_supply, get_self());
            }

            void send_issue_tokens(const asset& quantity, const string& memo, const name& contract ) {
                PRINT("vapaee::token::utils::send_issue_tokens()\n");
                action(
                    permission_level{vapaee::current_contract, "active"_n},
                    contract,
                    "issue"_n,
                    make_tuple(
                        vapaee::current_contract,
                        quantity,
                        memo
                    )
                ).send();
            }

            void send_issue_tokens(const asset& quantity, const string& memo) {
                PRINT("vapaee::token::utils::send_issue_tokens()\n");
                send_issue_tokens(quantity, memo, get_self());
            }

            void send_transfer_tokens(const name& from, const name& to, const asset& quantity, const string& memo, const name& contract) {
                PRINT("vapaee::token::utils::send_transfer_tokens()\n");
                action(
                    permission_level{vapaee::current_contract, "active"_n},
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

            void send_transfer_tokens(const name& from, const name& to, const asset& quantity, const string& memo) {
                PRINT("vapaee::token::utils::send_transfer_tokens()\n");
                name contract = vapaee::dex::token::get_asset_token_contract(quantity);
                send_transfer_tokens(from, to, quantity, memo, contract);
            }


            void send_debit_to_owner(const name& owner, const name& collector, const asset& quantity, const string& memo) {
                PRINT("vapaee::token::utils::send_debit_to_owner()\n");
                action(
                    permission_level{vapaee::current_contract, "active"_n},
                    get_self(),
                    name("debit"),
                    make_tuple(
                        owner,
                        collector,
                        quantity,
                        memo
                    )
                ).send();
            }

            void assert_token_registration(
                name action,
                const asset& quantity,
                const name& token_contract
            ) {
                PRINT("vapaee::token::utils::assert_token_registration()\n");
                
                vapaee::token::tokens tokens_table(get_self(), get_self().value);
                auto ptr = tokens_table.find(quantity.symbol.code().raw());
                if (ptr == tokens_table.end()) {
                    stats origin_stats_table( token_contract, quantity.symbol.code().raw() );
                    auto origin_token = origin_stats_table.find( quantity.symbol.code().raw() );

                    check(quantity.symbol == origin_token->max_supply.symbol,
                        create_error_asset2("ERR-ATR-01: Inconsistency found - \
                            The symbol of the quantity deposited differs from the symbol taken from the token contract max_supply property",
                            quantity,
                            origin_token->max_supply).c_str()
                        );
                
                    check(action == name("add") || action == name("set"),
                        create_error_name1("ERR-ATR-02: Invalid action", action).c_str()
                    );

                    tokens_table.emplace(get_self(), [&]( auto& a ){
                        a.supply = quantity;
                        a.account = token_contract;
                    });

                    if (get_self() != token_contract) {
                        vapaee::token::utils::send_create_token(origin_token->max_supply, get_self());
                    }
                    
                } else {
                    if (action == name("add")) {
                        tokens_table.modify(ptr, get_self(), [&]( auto& a ){
                            a.supply += quantity;
                        });
                    } else if (action == name("sub")) {
                        tokens_table.modify(ptr, get_self(), [&]( auto& a ){
                            asset sub = a.supply - quantity;
                            // check positive
                            check(a.supply.amount >= quantity.amount, create_error_asset2("ERR-ATR-03: Negative balance (current, taken)", a.supply, quantity).c_str());
                            a.supply = sub;
                        });
                    } else if (action == name("set")) {
                        tokens_table.modify(ptr, get_self(), [&]( auto& a ){
                            a.supply = quantity;
                        });
                    } else {
                        check(false, create_error_name1("ERR-ATR-02: Invalid action", action).c_str());
                    }
                }
            }

        };     
    };
};
