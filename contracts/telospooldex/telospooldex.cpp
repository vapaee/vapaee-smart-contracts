#include "telospooldex.hpp"

#include <vapaee/base/utils.hpp>
#include <vapaee/dex/tables/tokens.hpp>
#include <vapaee/dex/tables/markets.hpp>

using std::strtoull;
using std::to_string;

using vapaee::pool::telospooldex;
using vapaee::utils::split;


void telospooldex::initpool(name creator, uint64_t market_id) {
    require_auth(creator);
    markets book_markets(vapaee::dex::contract, vapaee::dex::contract.value);
    auto book_it = book_markets.find(market_id);
    check(book_it != book_markets.end(), ERR_MARKET_NOT_FOUND);

    pools pool_markets(get_self(), get_self().value);
    auto pool_it = pool_markets.find(market_id);
    check(pool_it == pool_markets.end(), ERR_POOL_EXISTS);

    tokens book_tokens(vapaee::dex::contract, vapaee::dex::contract.value);
    auto commodity_it = book_tokens.find(book_it->commodity.raw());
    auto currency_it = book_tokens.find(book_it->currency.raw());
    check(commodity_it != book_tokens.end(), ERR_TOKEN_NOT_REG);
    check(currency_it != book_tokens.end(), ERR_TOKEN_NOT_REG);

    pool_markets.emplace(get_self(), [&](auto& row) {
        row.id = market_id;
        row.commodity_reserve = asset(
                0, symbol(book_it->commodity, commodity_it->precision));
        row.currency_reserve = asset(
                0, symbol(book_it->currency, currency_it->precision));
    });
}


void telospooldex::tryfund(
    name funder, uint64_t market_id,
    symbol commodity, symbol currency
) {
    require_auth(funder);
    pools pool_markets(get_self(), get_self().value);
    auto pool_it = pool_markets.find(market_id);
    check(pool_it != pool_markets.end(), ERR_POOL_NOT_FOUND);
    check(pool_it->commodity_reserve.symbol == commodity, ERR_COMM_SYM_NOT_MATCH);
    check(pool_it->currency_reserve.symbol == currency, ERR_CURR_SYM_NOT_MATCH);

    check(is_account(funder), ERR_ACCOUNT_NOT_FOUND);

    fund_attempts funding_attempts(get_self(), get_self().value);
    funding_attempts.emplace(funder,  [&](auto & row) {
        row.id = funding_attempts.available_primary_key();
        row.pool_id = market_id;
        row.funder = funder;
        row.commodity = asset(0, commodity);
        row.currency = asset(0, currency);

        print(row.id);
    });
}


void telospooldex::cancelfund(name funder, uint64_t attempt_id) {
    require_auth(funder);

    fund_attempts funding_attempts(get_self(), get_self().value);
    auto fund_it = funding_attempts.find(attempt_id);
    check(fund_it != funding_attempts.end(), ERR_ATTEMPT_NOT_FOUND);
    check(fund_it->funder == funder, ERR_NOT_FUNDER);

    // cancel request is valid, return funds if any
    markets book_markets(vapaee::dex::contract, vapaee::dex::contract.value);
    auto book_it = book_markets.find(fund_it->pool_id);
    check(book_it != book_markets.end(), ERR_MARKET_NOT_FOUND);
    
    tokens book_tokens(vapaee::dex::contract, vapaee::dex::contract.value);
    auto commodity_it = book_tokens.find(book_it->commodity.raw());
    auto currency_it = book_tokens.find(book_it->currency.raw());
    check(commodity_it != book_tokens.end(), ERR_TOKEN_NOT_REG);
    check(currency_it != book_tokens.end(), ERR_TOKEN_NOT_REG);

    if (fund_it->commodity.amount > 0)
        action(
            permission_level{get_self(), "active"_n},
            commodity_it->contract,
            "transfer"_n,
            make_tuple(
                get_self(), funder,
                fund_it->commodity, "cancel fund attempt " + to_string(attempt_id)
            )
        ).send();

    if (fund_it->currency.amount > 0)
        action(
            permission_level{get_self(), "active"_n},
            currency_it->contract,
            "transfer"_n,
            make_tuple(
                get_self(), funder,
                fund_it->currency, "cancel fund attempt " + to_string(attempt_id)
            )
        ).send();

    funding_attempts.erase(fund_it);

}


void telospooldex::handle_transfer(
    name from,
    name to,
    asset quantity,
    string memo
) {
    if (from == get_self() ||  // skip if transaction comes from this contract
        to != get_self() )  // skip if contract is not target of transactions
        return;

    vector<string> tokens = split(memo, ",");
    check(tokens.size() == 2, ERR_MEMO_PARSING);

    // TODO: this is testing code till we can fund the pools using dao
    if (tokens[0] == "directfund") {
        uint64_t pool_id = strtoull(tokens[1].c_str(), nullptr, 10);
        
        pools pool_markets(get_self(), get_self().value);
        auto pool_it = pool_markets.find(pool_id);
        check(pool_it != pool_markets.end(), ERR_POOL_NOT_FOUND);

        if (quantity.symbol == pool_it->commodity_reserve.symbol)
            pool_markets.modify(pool_it, get_self(), [&](auto &row) {
                row.commodity_reserve += quantity;
            });

        if (quantity.symbol == pool_it->currency_reserve.symbol)
            pool_markets.modify(pool_it, get_self(), [&](auto &row) {
                row.currency_reserve += quantity;
            });

        return;
    }
    
    check(tokens[0] == "fund", ERR_MEMO_PARSING);

    uint64_t attempt_id = strtoull(tokens[1].c_str(), nullptr, 10);
    fund_attempts funding_attempts(get_self(), get_self().value);
    auto fund_it = funding_attempts.find(attempt_id);
    check(fund_it != funding_attempts.end(), ERR_ATTEMPT_NOT_FOUND);
    check(fund_it->funder == from, ERR_NOT_FUNDER);

    if (quantity.symbol == fund_it->commodity.symbol)
        funding_attempts.modify(fund_it, from, [&](auto &row) {
            row.commodity += quantity;
        });

    if (quantity.symbol == fund_it->currency.symbol)
        funding_attempts.modify(fund_it, from, [&](auto &row) {
            row.currency += quantity;
        });

    if (fund_it->commodity.amount == 0 || fund_it->currency.amount == 0)
        return;

    asset commodity_ex = asset_change_precision(
            fund_it->commodity, ARITHMETIC_PRECISION);
    asset currency_ex = asset_change_precision(
            fund_it->currency, ARITHMETIC_PRECISION);

    asset fund_rate = asset_divide(commodity_ex, currency_ex);
    
    if (fund_rate == get_exchange_rate(fund_it->pool_id)) {
        // TODO: emit fee participation token
        
        pools pool_markets(get_self(), get_self().value);
        auto pool_it = pool_markets.find(fund_it->pool_id);
        check(pool_it != pool_markets.end(), ERR_POOL_NOT_FOUND);

        pool_markets.modify(pool_it, get_self(), [&](auto &row) {
            row.commodity_reserve += fund_it->commodity;
            row.currency_reserve += fund_it->currency;
        });

        funding_attempts.erase(fund_it);
    }
}
