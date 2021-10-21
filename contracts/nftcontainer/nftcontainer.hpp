#pragma once

#include <vapaee/pool/utils.hpp>


namespace vapaee {

    namespace pool {

        class [[eosio::contract]] nftcontainer : public eosio::contract {

            private:
                #include <vapaee/pool/tables.all.hpp>

                pool_config config;

            public:
                using contract::contract;

                nftcontainer(name receiver, name code, datastream<const char*> ds) :
                    contract(receiver, code, ds),
                    config(receiver, receiver.value)
                    {}

                [[eosio::action]]
                void aaaaaa(name funder, uint64_t market_id);


        };  // contract class

    };  // namespace pool

};  // namespace vapaee
