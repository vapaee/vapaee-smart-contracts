#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pool/tables.hpp>

#define ARITHMETIC_PRECISION 8

#define ERR_MARKET_NOT_FOUND   "market not found"
#define ERR_POOL_NOT_FOUND     "pool not found"
#define ERR_POOL_EXISTS        "pool already exists"
#define ERR_TOKEN_NOT_REG      "token not registered"
#define ERR_COMM_SYM_NOT_MATCH "commodity symbols don\'t match"
#define ERR_CURR_SYM_NOT_MATCH "currency symbols don\'t match"
#define ERR_ACCOUNT_NOT_FOUND  "account not found"
#define ERR_MEMO_PARSING       "incorrect memo format"
#define ERR_ATTEMPT_NOT_FOUND  "fund attempt not found"
#define ERR_NOT_FUNDER         "not funder"

using eosio::asset;
using eosio::check;

using vapaee::utils::asset_divide;
using vapaee::utils::asset_change_precision;


namespace vapaee {

    namespace pool {

        asset get_exchange_rate(uint64_t pool_id) {
            pools pool_markets(contract, contract.value);
            auto pool_it = pool_markets.find(pool_id);
            check(pool_it != pool_markets.end(), ERR_POOL_NOT_FOUND);
            
            asset commodity_supply = asset_change_precision(
                    pool_it->commodity_reserve, ARITHMETIC_PRECISION);
            asset currency_supply = asset_change_precision(
                    pool_it->currency_reserve, ARITHMETIC_PRECISION);
            return asset_divide(commodity_supply, currency_supply);
        }

        class [[eosio::contract]] telospooldex : public eosio::contract {

            private:
                #include <vapaee/pool/tables.all.hpp> 

            public:
                using contract::contract;

                [[eosio::action]]
                void initpool(name creator, uint64_t market_id);

                [[eosio::action]]
                void tryfund(
                    name funder, uint64_t market_id,
                    symbol commodity, symbol currency);

                [[eosio::action]]
                void cancelfund(name funder, uint64_t attempt_id);

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
