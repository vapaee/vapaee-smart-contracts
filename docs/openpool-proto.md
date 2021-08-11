# open pool v1

## Fully decentralized token conversion protocol

Given a `converter` contract, the following *must* be implemented.

### Standard tables for `pools`

```C++
uint128_t symbols_get_index(symbol_code A, symbol_code B) {
    name a_name(to_lowercase(A.to_string()));
    name b_name(to_lowercase(B.to_string()));
    return ((uint128_t)a_name.value << 64) | a_name.value;
}

struct [[eosio::table]] pool_table {
    uint64_t id;
    asset commodity_reserve;
    asset currency_reserve;

    uint64_t primary_key() const { return id; }
    uint128_t by_symbols() const {
        return symbols_get_index(
            commodity_reserve.symbol.code(),
            currency_reserve.symbol.code()
        );
    }
    uint64_t by_commodity_key() const { return commodity_reserve.symbol.raw(); }
    uint64_t by_currency_key() const { return currency_reserve.symbol.raw(); }
};

// scope: contract.value
typedef eosio::multi_index<"pools"_n, pool_table,
    indexed_by<"symbols"_n, const_mem_fun<pool_table, uint128_t, &pool_table::by_symbols>>,
    indexed_by<"commodity"_n, const_mem_fun<pool_table, uint64_t, &pool_table::by_commodity_key>>,
    indexed_by<"currency"_n, const_mem_fun<pool_table, uint64_t, &pool_table::by_currency_key>>
> pools;
```

*the creation mechanism for pools is left as an implementation detail*

*the funding mechanism for pools is left as an implementation detail*

### Conversion protocol

Transfers to the `converter` contract using this protocol will trigger token
conversion mechanisms:

    from: any eosio account
    to: converter account
    asset: quantity
    memo: {protocol version},{path},{min},{recipient}

- protocol version: `"openpool.v1"`
- path: space separated strings following this format: 
    `{converter A}/{token A} {converter B}/{token B}`,
were `{converter A}` is a contract following this protocol with a pool that can
convert tokens from the sent tokens to the requested `{token A}`. Each `converter`
along this path will *pop* each space separated part and match the two values:

1. First the converter name must match `get_self()`
2. Use the sent quantity symbol + the symbol poped from the path to find the pool

If after the *pop* the path still has `{converter}/{token}` pairs, foward the
total of the transaction to next converter in path.

In case the path is empty after the *pop* send the total directly to `{recipient}`.

*TODO: set a standard for building a receipt of the trasaction each step of the path*
