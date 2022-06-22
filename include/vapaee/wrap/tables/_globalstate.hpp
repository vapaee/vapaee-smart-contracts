#pragma once
// -- globalstate --
// The global state of the contract is stored in this table

// scope: contract
// row: the global state of the contract

TABLE globalstate_table {
    // uint64_t current_shot;        // id of the last/current shot
    // uint64_t loans;               // total loans count

    // asset total_lendable;         // eosio.rexpool.total_lendable
    // asset total_rex;              // eosio.rexpool.total_rex
    // asset rex_balance;            // eosio.rexbal.rex_balance
    // asset matured_rex;            // eosio.rexbal.matured_rex
    
    // asset   total_assets;
    
    asset participation;
    // asset liquidity;              // amount of liquid TLOS ready for TLOSW redemption without touching the REX

    uint64_t primary_key() const { return 0; }
};

typedef eosio::multi_index<"globalstate"_n, globalstate_table> globalstate;
