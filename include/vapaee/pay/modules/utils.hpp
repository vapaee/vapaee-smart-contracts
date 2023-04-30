#pragma once
#include <vapaee/base/base.hpp>

namespace vapaee {
    namespace pay {
        namespace utils {

            inline name get_self() {
                return vapaee::pay::contract;
            }

            void send_leakpool(
                uint64_t leakpool_id
            ) {
                PRINT("vapaee::pay::utils::send_leakpool()\n");
                action(
                    permission_level{vapaee::current_contract, "active"_n},
                    get_self(),
                    "leakpool"_n,
                    make_tuple(
                        leakpool_id
                    )
                ).send();
            }


            void send_movepocket(
                uint64_t payhub_id,
                symbol_code sym_code
            ) {
                PRINT("vapaee::pay::utils::send_movepocket()\n");
                string target = string("pocket ") + std::to_string((long)payhub_id) + " " + sym_code.to_string();
                action(
                    permission_level{vapaee::current_contract, "active"_n},
                    get_self(),
                    name("movepocket"),
                    std::make_tuple(target, vapaee::current_contract)
                ).send();
            }

        };     
    };
};
