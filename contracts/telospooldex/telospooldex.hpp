#pragma once

#include <vapaee/pool/utils.hpp>


namespace vapaee {

    namespace pool {

        class [[eosio::contract]] telospooldex : public eosio::contract {

            private:
                #include <vapaee/pool/tables.all.hpp>

                pool_config config;

            public:
                using contract::contract;

                telospooldex(name receiver, name code, datastream<const char*> ds) :
                    contract(receiver, code, ds),
                    config(receiver, receiver.value)
                    {}

                [[eosio::action]]
                void cancelfund(name funder, uint64_t market_id);

                [[eosio::action]]
                void takepart(name funder, uint64_t market_id, asset score);

                [[eosio::action]]
                void setconfig(asset conversion_fee);

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
