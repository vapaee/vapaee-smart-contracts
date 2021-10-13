#include "./_aux.hpp"

#include <vapaee/base/utils.hpp>


// scope: contract
TABLE markets_table {
    uint64_t id;
    symbol_code commodity;
    symbol_code currency;
    uint64_t primary_key() const { return id; }
    uint64_t by_commodity_key() const { return commodity.raw(); }
    uint64_t by_currency_key() const { return currency.raw(); }

    uint128_t by_tokens_index() const {
        return vapaee::utils::symbols_get_index(commodity, currency);
    }

    string repr() const {
        return commodity.to_string() + "/" + currency.to_string();
    }
};

typedef eosio::multi_index<"markets"_n, markets_table,
    indexed_by<"commodity"_n, const_mem_fun<markets_table, uint64_t, &markets_table::by_commodity_key>>,
    indexed_by<"currency"_n, const_mem_fun<markets_table, uint64_t, &markets_table::by_currency_key>>,
    indexed_by<"tokensidx"_n, const_mem_fun<markets_table, uint128_t, &markets_table::by_tokens_index>>
> markets;
        
