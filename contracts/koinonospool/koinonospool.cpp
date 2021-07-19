#include "koinonospool.hpp"

using std::stoi;
using std::remove;
using std::make_tuple;

using eosio::check;
using eosio::action;
using eosio::symbol_code;
using eosio::require_auth;
using eosio::permission_level;


void koinonospool::initpool(
    name admin,
    name token_contract,
    symbol token_sym
) {

    require_auth(get_self());
    require_auth(admin);

    poolinfo_t pools(get_self(), get_self().value);

    auto sym_index = pools.get_index<"sym"_n>();
    auto pool_it = sym_index.find(token_sym.raw());
    check(pool_it == sym_index.end(), "pool for symbol already exists");

    pools.emplace(admin, [&](auto& row) {
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

        sym_index.modify(pool_it, pool_it->admin, [&](auto& row) {
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

    asset min_asset;
    min_asset.amount = stoi(str_amount);
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
    is_account(recipient);

    
    symbol from_symbol   = fpool_it->reserve.symbol;
    symbol to_symbol     = tpool_it->reserve.symbol;
    
    // extend assets to the best precision
    asset from_extended  = asset_best_precision(fpool_it->reserve, to_symbol);
    asset to_extended    = asset_best_precision(tpool_it->reserve, from_symbol);
    asset q_extended     = asset_best_precision(quantity, to_symbol);

    // get current exchange rate
    asset rate_extended  = asset(divide(to_extended, from_extended + q_extended), to_extended.symbol);

    // get equivalent of quantity sent in target tokens
    asset total_extended = asset_multiply(q_extended, rate_extended);
    asset total = asset_restore_precision(total_extended, to_symbol);

    check(total >= min_asset, BAD_DEAL_ERR);
    
    action(
        permission_level{get_self(), "active"_n},
        tpool_it->token_contract,
        "transfer"_n,
        make_tuple(get_self(), from, total, "koinonos.v1 rate: " + rate_extended.to_string())
    ).send();

    // update pool reserves
    sym_index.modify(fpool_it, get_self(), [&](auto& row) {
        row.reserve += quantity;
    });
    sym_index.modify(tpool_it, get_self(), [&](auto& row) {
        row.reserve -= total;
    });

    // string str = string("")
    // + " rate_extended: " + rate_extended.to_string()
    // + " from_extended: " + from_extended.to_string()
    // + " to_extended: " + to_extended.to_string()
    // + " q_extended: " + q_extended.to_string()
    // + " total_extended: " + total_extended.to_string()
    // + " total: " + total.to_string()
    // ;
    // eosio::check(false, str);

}
