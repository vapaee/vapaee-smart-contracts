#include "koinonospool.hpp"

using std::stoi;
using std::remove;
using std::make_tuple;

using eosio::check;
using eosio::action;
using eosio::symbol_code;
using eosio::require_auth;
using eosio::permission_level;

using vapaee::utils::ipow;
using vapaee::utils::split;
using vapaee::utils::asset_divide;
using vapaee::utils::asset_multiply;
using vapaee::utils::asset_change_precision;

void koinonospool::initpool(
    name admin,
    name token_contract,
    symbol token_sym
) {
    require_auth(get_self());

    poolinfo_t pools(get_self(), get_self().value);

    auto sym_index = pools.get_index<"sym"_n>();
    auto pool_it = sym_index.find(token_sym.raw());
    check(pool_it == sym_index.end(), "pool for symbol already exists");

    pools.emplace(get_self(), [&](auto& row) {
        row.id = pools.available_primary_key();
        row.admin = admin;
        row.token_contract = token_contract;
        row.reserve = asset(0, token_sym); 
    });
}

void koinonospool::transaction_handler(
    name from,
    name to,
    asset quantity, string memo
) {
    /*
     * skip handling if:
     *
     *  - transaction comes from self
     *  - transaction is not destined to self
     *  - memo is "skip"
     */
    if (from == get_self() ||
        to != get_self() ||
        memo == "skip")
        return;

    poolinfo_t pools(get_self(), get_self().value);
    auto sym_index = pools.get_index<"sym"_n>();

    if (memo == "fund") {
        auto pool_it = sym_index.find(quantity.symbol.raw());
        check(pool_it != sym_index.end(), POOL_NOT_FOUND_ERR);

        sym_index.modify(pool_it, get_self(), [&](auto& row) {
            row.reserve += quantity;
        });
        return;
    }

    check(!memo.empty(), EMPTY_MEMO_ERR);

    // koinonos protocol parsing
    vector<string> tokens = split(memo, ",");

    // https://github.com/vapaee/vapaee-smart-contracts/issues/46#issuecomment-879908663
    // <version>,<path>,<min>,<recipient>
    check(tokens.size() == 3, INCORRECT_MEMO_ERR);
    
    string version = tokens[0];
    check(version == PROTO_VERSION, WRONG_PROTOCOL_ERR);

    // minimun quantity asset parsing
    string min = tokens[1];
    vector<string> min_asset_tokens = split(min, " ");
    check(min_asset_tokens.size() == 2, MIN_FIELD_PARSE_ERR);

    string str_amount = min_asset_tokens[0];
    string str_symbol = min_asset_tokens[1];

    uint8_t min_asset_precision = str_amount.size() - str_amount.find('.') - 1;
    symbol min_asset_sym(
       symbol_code(str_symbol), min_asset_precision 
    );

    str_amount.erase(
        remove(str_amount.begin(), str_amount.end(), '.'),
        str_amount.end()
    );

    // quick patch to solve stoi memory issues on amounts higher than "2147483647"
    asset min_asset;
    int tolerance = 6;
    int64_t min_amount = 0;
    if (str_amount.length() > tolerance) {
        string upper = str_amount.substr(0, str_amount.length() - tolerance);
        string lower = str_amount.substr(str_amount.length() - tolerance);
        int64_t int_upper = stoi(upper);
        int64_t int_lower = stoi(lower);
        min_amount = int_upper * ipow(10, tolerance) + int_lower;
    } else {
        min_amount = stoi(str_amount);
    }

    min_asset.amount = min_amount;
    min_asset.symbol = min_asset_sym;

    // check both pools exist
    auto fpool_it = sym_index.find(quantity.symbol.raw());
    auto tpool_it = sym_index.find(min_asset.symbol.raw());
    check(fpool_it != sym_index.end(), POOL_NOT_FOUND_ERR);
    check(tpool_it != sym_index.end(), POOL_NOT_FOUND_ERR);

    // check both pools have reserves
    check(fpool_it->reserve.amount > 0, UNFUNDED_POOL_ERR);
    check(tpool_it->reserve.amount > 0, UNFUNDED_POOL_ERR);

    // validate recipient account & make transfer
    name recipient = name(tokens[2]);
    check(is_account(recipient), ACCOUNT_NOT_FOUND_ERR);

    asset to_pool_supply = asset_change_precision(tpool_it->reserve, ARITHMETIC_PRECISION);
    asset from_pool_supply = asset_change_precision(fpool_it->reserve, ARITHMETIC_PRECISION);
    asset quantity_ex = asset_change_precision(quantity, ARITHMETIC_PRECISION);

    // get current exchange rate
    asset rate = asset_divide(
        to_pool_supply,
        from_pool_supply + quantity_ex
    );

    // get equivalent of quantity sent in target tokens
    asset total = asset_multiply(quantity, rate);
    total = asset_change_precision(total, min_asset_sym.precision());

    print("rate:  ", rate, '\n');
    print("total: ", total, '\n');
    check(total >= min_asset, BAD_DEAL_ERR);
    
    action(
        permission_level{get_self(), "active"_n},
        tpool_it->token_contract,
        "transfer"_n,
        make_tuple(get_self(), from, total, "koinonos.v1 rate: " + rate.to_string())
    ).send();

    // update pool reserves
    sym_index.modify(fpool_it, get_self(), [&](auto& row) {
        row.reserve += quantity;
    });
    sym_index.modify(tpool_it, get_self(), [&](auto& row) {
        row.reserve -= total;
    });
}
