#pragma once
// -- snapshot --
// Each time a user lend or cancel a loan, a new snapshot of the REX is taken recording the state before the event and the state after.

// scope: contract
// row: a snapshot of the REX state

struct snapshot {
    uint64_t loans;               // total loans count

    asset   total_assets;         // gains calculated between this snapshot and the previous one;
    

    // asset total_lendable;         // eosio.rexpool.total_lendable
    // asset total_rex;              // eosio.rexpool.total_rex
    // asset rex_balance;            // eosio.rexbal.rex_balance
    // asset matured_rex;            // eosio.rexbal.matured_rex
    
    // tloswrap participation;    
};


TABLE snapshot_table {
    uint64_t id;                  // shot taked from global::current_shot
    snapshot before;              // state before this shot
    snapshot after;               // state after this shot
    asset   gains;                // gains from the last snapshot until this one;
    asset assigned;               // processed gains already assigned
    tloswrap participation;       // total participation between this snapshot and the previous one;
    std::deque<std::pair<time_point_sec, int64_t>> rex_maturities;  // is this useful?????
    uint64_t remaining;           // number of remaining loans to be processed; init rexshot::loans = global:loans. Decreases 1 each time a loan is proccessed on this rexshot.

    uint64_t primary_key() const { return id; }
};

typedef eosio::multi_index<"snapshots"_n, snapshot_table> snapshots;
