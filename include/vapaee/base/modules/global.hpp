#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/base/tables.hpp>

namespace vapaee {
    namespace base {
        namespace global {

            name get_self() {
                return vapaee::current_contract;
            }

            void action_init() {
                PRINT("vapaee::base::global::action_init()\n");
                version ver_table(get_self(), get_self().value);
                auto ver_itr = ver_table.begin();
                if (ver_itr == ver_table.end()) {
                    ver_table.emplace(get_self(), [&](auto &row) {
                        row.version = vapaee::current_version;
                    });
                } else {
                    if (ver_itr->version != vapaee::current_version) {
                        ver_table.modify(*ver_itr, get_self(), [&](auto &row) {
                            row.version = vapaee::current_version;
                        });
                    }
                }
            }

            bool handler_should_ignore_transfer(
                const name& from,
                const name& to,
                const asset& quantity,
                const string& memo,
                const name& token_contract                
            ) {
                PRINT("vapaee::base::global::handler_should_ignore_transfer()\n");

                // skip handling transfers from this contract to outside
                if (from == get_self()) return true;

                // skip handling transfers to third party
                if (to != get_self()) return true;

                // skip handling transfers with "skip" as memo
                if (memo == std::string("skip")) return true;

                // special cases caused by other contracts unther third party control
                if (token_contract == name("acornaccount") &&
                    memo == "UBI: " + quantity.to_string()   // Skip ACORN UBI
                ) {
                    return true;
                }

                return false;
            }
            
        };     
    };
};
