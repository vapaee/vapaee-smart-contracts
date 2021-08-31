#pragma once

#include <vapaee/pool/utils.hpp>


namespace vapaee {

    namespace pool {

        class [[eosio::contract]] telospooldex : public eosio::contract {

            private:
                #include <vapaee/pool/tables.all.hpp> 

            public:
                using contract::contract;

                [[eosio::action]]
                void cancelfund(name funder, uint64_t market_id);

                [[eosio::on_notify("*::transfer")]]
                void handle_transfer(
                    name from,
                    name to,
                    asset quantity,
                    string memo
                );

        };  // contract class

    };  // namespace pool

};  // namespace vapaee
