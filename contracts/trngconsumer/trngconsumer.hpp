#pragma once

#include <vapaee/base/base.hpp>
#include <vapaee/base/utils.hpp>
#include <vapaee/base/modules/global.hpp>
#include <vapaee/rng/modules/random.hpp>

namespace vapaee {

    CONTRACT trngconsumer : public eosio::contract {

        private:
            #include <vapaee/rng/tables.all.hpp>
            #include <vapaee/base/tables.all.hpp>
            
        public:
            using contract::contract;

            string get_version() { return string("0.0.1"); } // trngconsumer-0.0.1
            // token module

            ACTION init() {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::init() ------------------\n");
            }

            // request a random value
            ACTION requestrand(uint64_t seed, const name& caller, uint64_t min, uint64_t max) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::requestrand() ------------------\n");
                vapaee::rng::random::action_request_new_random(seed, caller, min, max);
            }
            
            ACTION recordrand(uint64_t request_id, name caller, uint64_t number) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::recordrand() ------------------\n");
                vapaee::rng::random::action_record_random_number(request_id, caller, number);
            }
            
            // action_delete_request
            ACTION delrequest(uint64_t request_id) {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::delrequest() ------------------\n");
                vapaee::rng::random::action_delete_request(request_id);
            }

            [[eosio::on_notify("rng.oracle::submitrand")]]
            ACTION receiverand(uint64_t caller_id, checksum256 random) {
                PRINT("\nHANDLER telospooldex::receiverand() ------------------\n");
                vapaee::rng::random::handler_receive_random_number(caller_id, random);
            }

            ACTION clear() {
                PRINT("\nACTION ",vapaee::current_contract.to_string(),"::clear() ------------------\n");
                require_auth(get_self()); // Only the contract itself can call this
                
                // Clear records
                records rec(get_self(), get_self().value);
                for (auto irec = rec.begin(); irec != rec.end(); irec = rec.erase(irec));

            }
            
    };  // contract class

};  // namespace vapaee
