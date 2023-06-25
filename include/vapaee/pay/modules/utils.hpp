#pragma once
#include <vapaee/base/base.hpp>

namespace vapaee {
    namespace pay {
        namespace utils {

            void send_payment_transfer(
                const string& target,
                const asset& quantity,
                const name& contract
            ) {
                PRINT("vapaee::pay::utils::send_payment_transfer()\n");

                string memo = string("pay ") + target;
                const name from = vapaee::current_contract;
                const name to = vapaee::pay::contract;

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
                PRINT("vapaee::pay::utils::send_payment_transfer()...\n");
            }



            void send_leakpool(
                uint64_t leakpool_id
            ) {
                PRINT("vapaee::pay::utils::send_leakpool()\n");
                action(
                    permission_level{vapaee::current_contract, "active"_n},
                    vapaee::pay::contract,
                    "leakpool"_n,
                    make_tuple(
                        leakpool_id
                    )
                ).send();
                PRINT("vapaee::pay::utils::send_leakpool()...\n");
            }


            void send_movepocket(
                uint64_t payhub_id,
                symbol_code sym_code
            ) {
                PRINT("vapaee::pay::utils::send_movepocket()\n");
                string target = string("pocket ") + std::to_string((long)payhub_id) + " " + sym_code.to_string();
                action(
                    permission_level{vapaee::current_contract, "active"_n},
                    vapaee::pay::contract,
                    name("movepocket"),
                    std::make_tuple(target, vapaee::current_contract)
                ).send();
            }

            void send_movepayment(
                string target,
                symbol_code token
            ) {
                PRINT("vapaee::pay::utils::send_movepayment()\n");
                action(
                    permission_level{vapaee::current_contract, "active"_n},
                    vapaee::pay::contract,
                    name("movepayment"),
                    std::make_tuple(target, token)
                ).send();
            }

            void send_shedule_payment(
                const asset& quantity,
                const string& target,
                const string& memo,
                const bool move
            ) {
                PRINT("vapaee::pay::utils::send_shedule_payment()\n");
                action(
                    permission_level{vapaee::current_contract, "active"_n},
                    vapaee::pay::contract,
                    name("pay"),
                    std::make_tuple(quantity, target, memo, move)
                ).send();
            }

            void send_shedule_payment(
                const asset& quantity,
                const string& target,
                const string& memo
            ) {
                send_shedule_payment(quantity, target, memo, false);
            }

        };     
    };
};
