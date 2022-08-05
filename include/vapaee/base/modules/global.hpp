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
            
        };     
    };
};
