// --- last24hs ---
// this is a summary of the market metriks for the last 24 hours.
// Each entry has a label with a patron "h.xxx" where xxx is the hour in english.
// there's a special entry "lastone" that represents the current hour.
// There will be allways 25 entries (24 * "h.xxx" + "lastone") and they are reused every 24h.

// scope: market-id
// row: represents a block of an hour for a given market (last 24 hours)

TABLE last24hs_table {
    name label;     // "lastone", "h.zero", "h.one", ..., "h.twentythree"
    asset price;    // current price for this hour (and last)
    asset inverse;
    asset entrance; // first price for this hour
    asset max;      // max price for this hour
    asset min;      // min price for this hour
    asset volume;
    asset amount;
    uint64_t hour;  // is a numeric counter that increments every hour (calculated from date)
    time_point_sec date;
    uint64_t primary_key() const { return label.value; }
};

typedef eosio::multi_index< "last24hs"_n, last24hs_table > last24hs;
