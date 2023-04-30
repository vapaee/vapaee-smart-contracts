// -- leakpools --
// PayHub provides with the service of distributing a specific amount of assets in a specific period of time.
// This assets are lock in leakpools and leaked over time. This leakpool redirect all the assets leaked to a specific payhub.

// scope: contract
// row: represents a leakpool 

TABLE leakpools_table {
    uint64_t id;
    uint64_t paygub;                    // payhub that receive the funds
    name admin;

    string title;                       // Readable title

    asset total;                        // total ever (left + leaked)
    asset left;                         // total available funds (total = liquid + allowed)
    asset liquid;                       // Available liquid funds
    asset allowed;                      // Available allowed future liquidity to issue (exclusively for vapaeetokens native Tokens)
    asset leaked;                       // Leaked so far
    asset delta;                        // Last leak

    time_point_sec start;               // When pool started to leak
    time_point_sec end;                 // When pool should finish to leak the last token
    time_point_sec last_leak;           // When pool was last leaked

    name easing;                        // Name of the easing function to leak tokens
    
    uint64_t primary_key()const { return id; }
    uint64_t by_lastleaked()const { return (uint64_t) (time_point_sec::maximum().sec_since_epoch() - last_leak.sec_since_epoch()); }
};

typedef eosio::multi_index<"leakpools"_n, leakpools_table,
    indexed_by<"lastleaked"_n, const_mem_fun<leakpools_table, uint64_t, &leakpools_table::by_lastleaked>>
> leakpools;
