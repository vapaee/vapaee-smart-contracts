#pragma once

#include <vapaee/base/base.hpp>
#include <vapaee/dex/modules/utils.hpp>
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
#define ERR_FAKE_TOKEN          "wrong token contract"

#define PROTO_VERSION "openpool.v1"_n

#define ECHO_CONTRACT "echocontract"_n

#define THANK_YOU_MSG "Thank you for using Telos Pool DEX"


using eosio::asset;
using eosio::check;

using vapaee::dex::utils::get_contract_for_token;
using vapaee::utils::pack;
using vapaee::utils::asset_divide;
using vapaee::utils::symbols_get_index;
using vapaee::utils::asset_change_precision;


namespace vapaee {

    namespace pool {

        namespace utils {

            asset get_pool_rate(uint64_t pool_id) {
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
                return sym_index.find(symbols_get_index(A, B)) != sym_index.end();
            }

            void create_fund_attempt(name funder, uint64_t market_id) {
                require_auth(funder);
                pools pool_markets(contract, contract.value);
                auto pool_it = pool_markets.find(market_id);
                check(pool_it != pool_markets.end(), ERR_POOL_NOT_FOUND);
                check(is_account(funder), ERR_ACCOUNT_NOT_FOUND);

                fund_attempts funding_attempts(contract, contract.value);
                funding_attempts.emplace(contract, [&](auto & row) {
                    row.auto_id = funding_attempts.available_primary_key();
                    row.id = pack(funder.value, market_id);
                    row.commodity = asset(0, pool_it->commodity_reserve.symbol);
                    row.currency = asset(0, pool_it->currency_reserve.symbol);
                });
            }

            void maybe_end_fund_attempt(name funder, uint64_t market_id) {
                fund_attempts funding_attempts(contract, contract.value);
                auto pack_index = funding_attempts.get_index<"idpacked"_n>();
                auto fund_it = pack_index.find(pack(funder.value, market_id));
                
                check(fund_it != pack_index.end(), ERR_ATTEMPT_NOT_FOUND);

                asset commodity_ex = asset_change_precision(
                        fund_it->commodity, ARITHMETIC_PRECISION);
                asset currency_ex = asset_change_precision(
                        fund_it->currency, ARITHMETIC_PRECISION);

                asset fund_rate = asset_divide(commodity_ex, currency_ex);
                
                if (fund_rate == get_pool_rate(fund_it->get_pool_id())) {
                    // TODO: emit fee participation token
                    
                    pools pool_markets(contract, contract.value);
                    auto pool_it = pool_markets.find(fund_it->get_pool_id());
                    check(pool_it != pool_markets.end(), ERR_POOL_NOT_FOUND);

                    pool_markets.modify(pool_it, contract, [&](auto &row) {
                        row.commodity_reserve += fund_it->commodity;
                        row.currency_reserve += fund_it->currency;
                    });

                    pack_index.erase(fund_it);
                }
            }

        };  // namespace utils

    };  // namespace pool

};  // namespace vapaee
