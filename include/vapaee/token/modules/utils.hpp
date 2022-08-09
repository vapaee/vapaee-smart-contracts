#pragma once
#include <vapaee/base/base.hpp>

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
                send_burn_tokens(quantity, memo, vapaee::token::contract);
            }


            void send_create_token(const asset& max_supply, const name& contract) {
                PRINT("vapaee::token::utils::send_create_token()\n");
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
                send_create_token(max_supply, vapaee::token::contract);
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
                send_issue_tokens(quantity, memo, vapaee::token::contract);
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
                send_transfer_tokens(from, to, quantity, memo, vapaee::token::contract);
            }


            void send_debit_to_owner(const name& owner, const name& collector, const asset& quantity, const string& memo) {
                PRINT("vapaee::token::utils::send_debit_to_owner()\n");
                action(
                    permission_level{vapaee::current_contract, "active"_n},
                    vapaee::token::contract,
                    name("debit"),
                    make_tuple(
                        owner,
                        collector,
                        quantity,
                        memo
                    )
                ).send();
            }


        };     
    };
};
