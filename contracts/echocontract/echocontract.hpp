#pragma once
#include <vapaee/base/base.hpp>

using namespace eosio;


class [[eosio::contract]] echocontract : public eosio::contract {
    public:
        using contract::contract;

        [[eosio::action]]
        void noop() {}

        [[eosio::on_notify("*::transfer")]]
        void handle_transfer(
            name from,
            name to,
            asset quantity,
            string memo
        ) {
            if (from == get_self() ||
                to != get_self())
                return;

            action(
                permission_level{get_self(), "active"_n},
                get_first_receiver(),
                "transfer"_n, 
                make_tuple(get_self(), from, quantity, memo)
            ).send();
        }
};
