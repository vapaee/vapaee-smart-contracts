#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/rng/errors.hpp>
#include <vapaee/rng/tables.hpp>
#include <vapaee/dex/modules/security.hpp>
#include <vapaee/pay/modules/utils.hpp>

static constexpr eosio::name ORACLE_CONTRACT = name("rng.oracle");

namespace vapaee {
    namespace rng {
        namespace random {

            inline name get_self() {
                return vapaee::rng::contract;
            }
            void action_request_new_random(uint64_t seed, const name& caller, uint64_t min, uint64_t max) {
                PRINT("vapaee::rng:random::action_request_new_random()\n");
                PRINT(" seed: ", std::to_string((unsigned long)seed), "\n");
                PRINT(" caller: ", caller.to_string(), "\n");
                PRINT(" min: ", std::to_string((unsigned long)min), "\n");
                PRINT(" max: ", std::to_string((unsigned long)max), "\n");
                //validate
                require_auth(caller);
                //open requests table, find request
                records rec_table(get_self(), get_self().value);
                auto request_id = rec_table.available_primary_key();
                //emplace new request
                rec_table.emplace(caller, [&](auto& col) {
                    col.request_id = request_id;
                    col.min = min;
                    col.max = max;
                    col.caller = caller;
                });
                action(
                    {get_self(), "active"_n},
                    ORACLE_CONTRACT, "requestrand"_n,
                    std::tuple(request_id, seed, get_self()))
                .send();
            }

            void action_record_random_number(uint64_t request_id, name caller, uint64_t number) {
                PRINT("vapaee::rng:random::action_record_random_number()\n");
                PRINT(" request_id: ", std::to_string((unsigned long)request_id), "\n");
                PRINT(" caller: ", caller.to_string(), "\n");
                PRINT(" number: ", std::to_string((unsigned long)number), "\n");
                require_auth(get_self());
                records rec_table(get_self(), get_self().value);
                auto req_itr = rec_table.find(request_id);
                check(req_itr != rec_table.end(), "Unable to find request");
                rec_table.modify(req_itr, get_self(), [&](auto& col) {
                    col.number = number;
                });
            }

            void action_delete_request(uint64_t request_id) {
                PRINT("vapaee::rng:random::action_delete_request()\n");
                PRINT(" request_id: ", std::to_string((unsigned long)request_id), "\n");
                require_auth(get_self());
                records rec_table(get_self(), get_self().value);
                auto req_itr = rec_table.find(request_id);
                check(req_itr != rec_table.end(), "Unable to find request");
                rec_table.erase(req_itr);
            }

            void handler_receive_random_number(uint64_t caller_id, checksum256 random) {
                PRINT("vapaee::rng:random::handler_receive_random_number()\n");
                PRINT(" caller_id: ", std::to_string((unsigned long)caller_id), "\n");
                require_auth(ORACLE_CONTRACT);
                records rec_table(get_self(), get_self().value);
                auto req_itr = rec_table.find(caller_id);
                if (req_itr != rec_table.end()) {
                    auto byte_array = random.extract_as_byte_array();
                    uint64_t random_int = 0;
                    for (int i = 0; i < 8; i++) {
                        random_int <<= 8;
                        random_int |= (uint64_t)byte_array[i];
                    }
                    auto min = req_itr->min;
                    auto max = req_itr->max;
                    uint64_t number = min + (random_int % (max - min + 1));
                    action(
                        {get_self(), "active"_n},
                        get_self(), "recordrand"_n,
                        std::tuple(req_itr->request_id, req_itr->caller, number))
                    .send();
                }
            }


        };     
    };
};
