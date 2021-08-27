#include "telospooldex.hpp"

#include <vapaee/base/utils.hpp>

using std::strtoull;
using std::to_string;

using vapaee::pool::pool_exists;
using vapaee::pool::telospooldex;
using vapaee::utils::split;


void telospooldex::createpool(name creator, uint64_t market_id) {
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

    check(
        !pool_exists(book_it->commodity, book_it->currency) &&
        !pool_exists(book_it->currency, book_it->commodity),
        ERR_POOL_EXISTS
    );

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

    if (fund_it->commodity.amount > 0)
        action(
            permission_level{get_self(), "active"_n},
            get_contract_for_token(book_it->commodity),
            "transfer"_n,
            make_tuple(
                get_self(), funder,
                fund_it->commodity, "cancel fund attempt " + to_string(attempt_id)
            )
        ).send();

    if (fund_it->currency.amount > 0)
        action(
            permission_level{get_self(), "active"_n},
            get_contract_for_token(book_it->currency),
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
        to != get_self() ||  // skip if contract is not target of transactions
        memo == "skip")
        return;

    vector<string> memo_tokens = split(memo, ",");
    check(memo_tokens.size() > 0, ERR_MEMO_PARSING);

    switch(name(memo_tokens[0]).value) {

        case "directfund"_n.value : {
            // TODO: this is testing code to do initial funding 
            check(memo_tokens.size() == 2, ERR_MEMO_PARSING);
            uint64_t pool_id = strtoull(memo_tokens[1].c_str(), nullptr, 10);
            
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

        case "fund"_n.value : {
            check(memo_tokens.size() == 2, ERR_MEMO_PARSING);
            uint64_t attempt_id = strtoull(memo_tokens[1].c_str(), nullptr, 10);
            fund_attempts funding_attempts(get_self(), get_self().value);
            auto fund_it = funding_attempts.find(attempt_id);
            check(fund_it != funding_attempts.end(), ERR_ATTEMPT_NOT_FOUND);
            check(fund_it->funder == from, ERR_NOT_FUNDER);

            if (quantity.symbol == fund_it->commodity.symbol) {
                check(
                    get_first_receiver() == get_contract_for_token(fund_it->commodity.symbol.code()),
                    ERR_FAKE_TOKEN); 
                funding_attempts.modify(fund_it, from, [&](auto &row) {
                    row.commodity += quantity;
                });
            }

            if (quantity.symbol == fund_it->currency.symbol) {
                check(
                    get_first_receiver() == get_contract_for_token(fund_it->currency.symbol.code()),
                    ERR_FAKE_TOKEN); 
                funding_attempts.modify(fund_it, from, [&](auto &row) {
                    row.currency += quantity;
                });
            }

            if (fund_it->commodity.amount == 0 || fund_it->currency.amount == 0)
                return;

            asset commodity_ex = asset_change_precision(
                    fund_it->commodity, ARITHMETIC_PRECISION);
            asset currency_ex = asset_change_precision(
                    fund_it->currency, ARITHMETIC_PRECISION);

            asset fund_rate = asset_divide(commodity_ex, currency_ex);
            
            if (fund_rate == get_pool_rate(fund_it->pool_id)) {
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
            return;
        }

        case PROTO_VERSION.value : {  
            check(memo_tokens.size() == 4, ERR_MEMO_PARSING);
            // general protocol parsing
            asset min = asset_from_string(memo_tokens[2]);
            name recipient = name(memo_tokens[3]);
            check(is_account(recipient), ERR_RECIPIENT_NOT_FOUND);
            
            // get first element of path
            string path_str = memo_tokens[1];
            vector<string> jumps = split(path_str, " ");
            check(jumps.size() > 0, ERR_EMPTY_PATH);

            vector<string> conversion_data = split(jumps.front(), "/");
            check(conversion_data.size() == 2, ERR_MEMO_PARSING);
            check(name(conversion_data[0]) == get_self(), ERR_INCORRECT_CONVERTER);

            // find pool
            symbol_code A = quantity.symbol.code();
            symbol_code B = symbol_code(conversion_data[1]);

            pools pool_markets(get_self(), get_self().value);
            auto sym_index = pool_markets.get_index<"symbols"_n>();
            auto pool_it = sym_index.find(symbols_get_index(A, B));

            if (pool_it == sym_index.end())
                pool_it = sym_index.find(symbols_get_index(B, A));

            check(pool_it != sym_index.end(), ERR_POOL_NOT_FOUND);

            // check if pool has funds
            check(pool_it->commodity_reserve.amount > 0, ERR_POOL_NOT_FUNDED);
            check(pool_it->currency_reserve.amount > 0, ERR_POOL_NOT_FUNDED);

            // make conversion
            tuple<asset, asset> result = get_conversion(pool_it->id, quantity);
            asset total = get<0>(result);
            asset rate = get<1>(result);
  
            // update pool reserves
            sym_index.modify(pool_it, get_self(), [&](auto& row) {
                if (quantity.symbol.code() == row.commodity_reserve.symbol.code()) {
                    row.commodity_reserve += quantity;
                    row.currency_reserve -= total;
                } else {
                    row.currency_reserve += quantity;
                    row.commodity_reserve -= total;
                }
            });

            // pop first
            jumps.erase(jumps.begin());

            if (jumps.size() == 0) {
                // last jump of path, send tokens to recipient

                print("total: ", total, '\n');
                print("rate: ", rate, '\n');
                check(total >= min, ERR_BAD_DEAL);
               
                // TODO: final transaction memo
                action(
                    permission_level{get_self(), "active"_n},
                    get_contract_for_token(total.symbol.code()),
                    "transfer"_n, 
                    make_tuple(
                        get_self(), recipient, total, string(""))
                ).send();
                return;
            }

            // still more jumps to go
            vector<string> next_conversion_data = split(jumps.front(), "/");
            check(next_conversion_data.size() == 2, ERR_MEMO_PARSING);

            name next_converter = name(next_conversion_data[0]);
            check(is_account(next_converter), ERR_CONVERTER_NOT_FOUND);

            vector<string> memo_parts;
            memo_parts.push_back(PROTO_VERSION.to_string());
            memo_parts.push_back(join(jumps, " "));
            memo_parts.push_back(min.to_string());
            memo_parts.push_back(recipient.to_string());

            // in case the next jump is towards self, send to echocontract
            if (next_converter == get_self())
                next_converter = ECHO_CONTRACT;

            action(
                permission_level{get_self(), "active"_n},
                get_contract_for_token(total.symbol.code()),
                "transfer"_n,
                make_tuple(
                    get_self(), next_converter, total, join(memo_parts, ","))
            ).send();

            break;
        }

        default : check(false, ERR_MEMO_PARSING);
    }
    
}
