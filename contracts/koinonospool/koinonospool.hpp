#pragma once

#include <string>

#include <vapaee/base/utils.hpp>

#include "errors.hpp"


using std::string;

using eosio::name;
using eosio::asset;
using eosio::symbol;
using eosio::indexed_by;
using eosio::multi_index;
using eosio::const_mem_fun;

static const char* PROTO_VERSION = "koinonos.v1";

static const uint8_t ARITHMETIC_PRECISION = 8;

/*
 *
 * koinonospool protocol version 1
 *
 * transactions to this contract must use the following memo format
 *
 * "<version>,<min>,<recipient>"
 *
 * min: asset, minimum expected to convert, desired sym is extracted from here
 * recipient: valid eosio name
 *
 */


class [[eosio::contract("koinonospool")]] koinonospool : public eosio::contract {
    public:
        using contract::contract;

        [[eosio::action]]
        void initpool(
            name admin,
            name token_contract,
            symbol token_sym
        );

        [[eosio::on_notify("*::transfer")]]
        void transaction_handler(
            name from,
            name to,
            asset quantity, string memo
        );

    private:
        struct [[eosio::table]] poolinfo_s {
            uint64_t id;
            name     admin;
            name     token_contract;
            asset    reserve;

            uint64_t primary_key() const { return id; }

            uint64_t by_symbol() const { return reserve.symbol.raw(); }
        };

        typedef multi_index<"pools"_n, poolinfo_s,
            indexed_by<"sym"_n, const_mem_fun<poolinfo_s, uint64_t, &poolinfo_s::by_symbol>>
        > poolinfo_t;
};

