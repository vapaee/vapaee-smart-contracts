#pragma once

#include <vapaee/base/base.hpp>
#include <vapaee/dex/tables.hpp>
#include <vapaee/pool/tables.hpp>

#define ARITHMETIC_PRECISION 8

#define ERR_MARKET_NOT_FOUND    "market not found"
#define ERR_POOL_NOT_FOUND      "pool not found"
#define ERR_POOL_EXISTS         "pool already exists"
#define ERR_TOKEN_NOT_REG       "token not registered"
#define ERR_COMM_SYM_NOT_MATCH  "commodity symbols don\'t match"
#define ERR_CURR_SYM_NOT_MATCH  "currency symbols don\'t match"
#define ERR_ACCOUNT_NOT_FOUND   "account not found"
#define ERR_MEMO_PARSING        "incorrect memo format"
#define ERR_ATTEMPT_NOT_FOUND   "fund attempt not found"
#define ERR_NOT_FUNDER          "not funder"
#define ERR_EMPTY_PATH          "path is empty"
#define ERR_INCORRECT_CONVERTER "this is not the converter you need"
#define ERR_POOL_NOT_FUNDED     "unfunded pool"
#define ERR_RECIPIENT_NOT_FOUND "recipient not found"
#define ERR_BAD_DEAL            "total less than minimun"
#define ERR_CONVERTER_NOT_FOUND "can't find converter"

#define PROTO_VERSION "openpool.v1"_n

#define ECHO_CONTRACT "echocontract"_n


using eosio::asset;
using eosio::check;

using vapaee::dex::tokens;
using vapaee::utils::asset_divide;
using vapaee::utils::symbols_get_index;
using vapaee::utils::asset_change_precision;


namespace vapaee {

    namespace pool {

        name get_contract_for_token(symbol_code sym) {
            tokens book_tokens(vapaee::dex::contract, vapaee::dex::contract.value);
            auto book_it = book_tokens.find(sym.raw());
            check(book_it != book_tokens.end(), ERR_TOKEN_NOT_REG);
            return book_it->contract;
        }

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

        tuple<asset, asset> get_conversion(
            uint64_t pool_id, asset quantity
        ) {
            pools pool_markets(contract, contract.value);
            auto pool_it = pool_markets.find(pool_id);
            check(pool_it != pool_markets.end(), ERR_POOL_NOT_FOUND);
            
            asset from_reserve, to_reserve;
            if (quantity.symbol.code() == 
                    pool_it->commodity_reserve.symbol.code()) {
                from_reserve = pool_it->commodity_reserve;
                to_reserve = pool_it->currency_reserve;
            } else {
                from_reserve = pool_it->currency_reserve;
                to_reserve = pool_it->commodity_reserve;
            }
            
            asset from_reserve_ex = asset_change_precision(from_reserve, ARITHMETIC_PRECISION);
            asset to_reserve_ex = asset_change_precision(to_reserve, ARITHMETIC_PRECISION);
            asset quantity_ex = asset_change_precision(quantity, ARITHMETIC_PRECISION);

            asset rate = asset_divide(
                to_reserve_ex, from_reserve_ex + quantity_ex);

            asset conversion_ex = asset_multiply(quantity, rate);
            asset conversion = asset_change_precision(
                conversion_ex, to_reserve.symbol.precision());

            return make_tuple(conversion, rate);
        }

        bool pool_exists(symbol_code A, symbol_code B) {
            pools pool_markets(contract, contract.value);
            auto sym_index = pool_markets.get_index<"symbols"_n>();
            return sym_index.find(symbols_get_index(A, B).value) != sym_index.end();
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
