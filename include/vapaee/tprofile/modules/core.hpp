#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/tprofile/tables.hpp>

namespace vapaee {
    namespace tprofile {
        namespace core {

            void action_add_platform(string platform) {
                require_auth(contract);

                platforms plat_table(contract, contract.value);
                auto pname_index = plat_table.get_index<"pname"_n>();
                auto plat_iter = pname_index.find(vapaee::utils::hash(platform));
                check(plat_iter == pname_index.end(), "identical platform exists");

                plat_table.emplace(contract, [&](auto& row) {
                    row.id = plat_table.available_primary_key();
                    row.pname = platform;
                    row.counter = 0;
                });
            }

        };     
    };
};
