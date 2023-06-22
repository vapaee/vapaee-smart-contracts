#!/bin/bash
CONTRACT="telosbookdex"

# determine SCRIPT_DIR and import lib
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../" && pwd)"
source $SCRIPT_DIR/_lib.sh

function compile() {
    CONTRACT="$1"
    # check if $CONTRACT.wasm needs to be recompiled
    if [[ ! -f "$HOME/contracts/$CONTRACT/build/$CONTRACT.wasm" ||
        "$HOME/contracts/$CONTRACT/$CONTRACT.hpp" -nt "$HOME/contracts/$CONTRACT/build/$CONTRACT.wasm" ||
        "$HOME/contracts/$CONTRACT/$CONTRACT.cpp" -nt "$HOME/contracts/$CONTRACT/build/$CONTRACT.wasm" ||
        "$FORCE" == true
    ]]; then
        print_title "-- $CONTRACT --"
        compile_contract "$CONTRACT"
    fi
}

# si alguno de los parámetros es compile, entonces compilamos
if [[ "$@" =~ "compile" ]]; then
    compile "$CONTRACT"
fi

## -- deploy --
function deploy() {
    CONTRACT="$1"
    print_title "--- Deploying $CONTRACT ---"
    deploy_contract "$CONTRACT"
}

# si alguno de los parámetros es deploy, entonces deployamos
if [[ "$@" =~ "deploy" ]]; then
    deploy "$CONTRACT"
fi

## -- init --
function init() {
    
    # ACTION newmarket(
    #     const symbol_code & commodity,
    #     const symbol_code & currency,
    #     name converter
    # )

    # Market list:

    # cleos_push_action telosmaindex newmarket '["TLOS", "EUROT", "telospooldex"]' -p telosmaindex
    # cleos_push_action telosmaindex newmarket '["VPE", "EUROT", "telospooldex"]' -p telosmaindex
    # cleos_push_action telosmaindex newmarket '["CNT", "EUROT", "telospooldex"]' -p telosmaindex
    # cleos_push_action telosmaindex newmarket '["TIPS", "EUROT", "telospooldex"]' -p telosmaindex
    # cleos_push_action telosmaindex newmarket '["USDT", "EUROT", "telospooldex"]' -p telosmaindex
    # cleos_push_action telosmaindex newmarket '["KOINE", "EUROT", "telospooldex"]' -p telosmaindex
    # cleos_push_action telosmaindex newmarket '["ACORN", "EUROT", "telospooldex"]' -p telosmaindex
    # cleos_push_action telosmaindex newmarket '["HEART", "EUROT", "telospooldex"]' -p telosmaindex
    # cleos_push_action telosmaindex newmarket '["MULITA", "EUROT", "telospooldex"]' -p telosmaindex
    # cleos_push_action telosmaindex newmarket '["DIVERSE", "EUROT", "telospooldex"]' -p telosmaindex

    print_title "--- init telosbookdex ---"
}

# si alguno de los parámetros es init, entonces cargamos los datos
if [[ "$@" =~ "init" ]]; then
    init "$CONTRACT"
fi


## -- load data --
function loaddata() {
    print_title "--- Adding orders to Telosbookdex ---"

 
    # Deposit some tokens in the exchange
    cleos_push_action eosio.token  transfer '["tokenissuer", "telosbookdex", "100.0000 TLOS", "deposit"]' -p tokenissuer
    cleos_push_action tokens.swaps transfer '["tokenissuer", "telosbookdex", "100.0000 USDT", "deposit"]' -p tokenissuer
    cleos_push_action acornaccount transfer '["tokenissuer", "telosbookdex", "100.0000 ACORN", "deposit"]' -p tokenissuer
    cleos_push_action revelation21 transfer '["tokenissuer", "telosbookdex", "100.0000 HEART", "deposit"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telosbookdex", "100.000000 VPE", "deposit"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telosbookdex", "100.0000 CNT", "deposit"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telosbookdex", "100.000000 TIPS", "deposit"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telosbookdex", "100.000000 KOINE", "deposit"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telosbookdex", "1000.0000 EUROT", "deposit"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telosbookdex", "100.00 MULITA", "deposit"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telosbookdex", "100.000000 DIVERSE", "deposit"]' -p tokenissuer

    # ACTION order(
    #     name owner,
    #     name type,
    #     const asset & total,
    #     const asset & price,
    #     uint64_t client
    # )

    print_subtitle "-- TLOS / EUROT --"
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 TLOS", "1.50000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 TLOS", "1.40000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 TLOS", "1.30000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 TLOS", "1.20000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 TLOS", "1.10000000 EUROT", 0]' -p tokenissuer

    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 TLOS", "0.90000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 TLOS", "0.80000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 TLOS", "0.70000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 TLOS", "0.60000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 TLOS", "0.50000000 EUROT", 0]' -p tokenissuer

    print_subtitle "-- VPE / EUROT --"
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 VPE", "1.50000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 VPE", "1.40000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 VPE", "1.30000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 VPE", "1.20000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 VPE", "1.10000000 EUROT", 0]' -p tokenissuer

    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 VPE", "0.90000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 VPE", "0.80000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 VPE", "0.70000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 VPE", "0.60000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 VPE", "0.50000000 EUROT", 0]' -p tokenissuer

    print_subtitle "-- CNT / EUROT --"
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 CNT", "1.50000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 CNT", "1.40000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 CNT", "1.30000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 CNT", "1.20000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 CNT", "1.10000000 EUROT", 0]' -p tokenissuer

    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 CNT", "0.90000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 CNT", "0.80000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 CNT", "0.70000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 CNT", "0.60000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 CNT", "0.50000000 EUROT", 0]' -p tokenissuer

    print_subtitle "-- TIPS / EUROT --"
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 TIPS", "1.50000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 TIPS", "1.40000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 TIPS", "1.30000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 TIPS", "1.20000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 TIPS", "1.10000000 EUROT", 0]' -p tokenissuer

    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 TIPS", "0.90000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 TIPS", "0.80000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 TIPS", "0.70000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 TIPS", "0.60000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 TIPS", "0.50000000 EUROT", 0]' -p tokenissuer

    print_subtitle "-- USDT / EUROT --"
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 USDT", "1.50000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 USDT", "1.40000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 USDT", "1.30000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 USDT", "1.20000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 USDT", "1.10000000 EUROT", 0]' -p tokenissuer

    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 USDT", "0.90000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 USDT", "0.80000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 USDT", "0.70000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 USDT", "0.60000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 USDT", "0.50000000 EUROT", 0]' -p tokenissuer

    print_subtitle "-- KOINE / EUROT --"
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 KOINE", "1.50000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 KOINE", "1.40000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 KOINE", "1.30000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 KOINE", "1.20000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 KOINE", "1.10000000 EUROT", 0]' -p tokenissuer

    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 KOINE", "0.90000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 KOINE", "0.80000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 KOINE", "0.70000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 KOINE", "0.60000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 KOINE", "0.50000000 EUROT", 0]' -p tokenissuer

    print_subtitle "-- ACORN / EUROT --"
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 ACORN", "1.50000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 ACORN", "1.40000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 ACORN", "1.30000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 ACORN", "1.20000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 ACORN", "1.10000000 EUROT", 0]' -p tokenissuer

    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 ACORN", "0.90000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 ACORN", "0.80000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 ACORN", "0.70000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 ACORN", "0.60000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 ACORN", "0.50000000 EUROT", 0]' -p tokenissuer

    print_subtitle "-- HEART / EUROT --"
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 HEART", "1.50000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 HEART", "1.40000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 HEART", "1.30000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 HEART", "1.20000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 HEART", "1.10000000 EUROT", 0]' -p tokenissuer

    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 HEART", "0.90000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 HEART", "0.80000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 HEART", "0.70000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 HEART", "0.60000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 HEART", "0.50000000 EUROT", 0]' -p tokenissuer

    print_subtitle "-- MULITA / EUROT --"
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 MULITA", "1.50000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 MULITA", "1.40000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 MULITA", "1.30000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 MULITA", "1.20000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 MULITA", "1.10000000 EUROT", 0]' -p tokenissuer

    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 MULITA", "0.90000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 MULITA", "0.80000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 MULITA", "0.70000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 MULITA", "0.60000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 MULITA", "0.50000000 EUROT", 0]' -p tokenissuer

    print_subtitle "-- DIVERSE / EUROT --"
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 DIVERSE", "1.50000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 DIVERSE", "1.40000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 DIVERSE", "1.30000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 DIVERSE", "1.20000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "sell", "10.00000000 DIVERSE", "1.10000000 EUROT", 0]' -p tokenissuer

    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 DIVERSE", "0.90000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 DIVERSE", "0.80000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 DIVERSE", "0.70000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 DIVERSE", "0.60000000 EUROT", 0]' -p tokenissuer
    cleos_push_action telosbookdex order '["tokenissuer", "buy",  "10.00000000 DIVERSE", "0.50000000 EUROT", 0]' -p tokenissuer


}

# si alguno de los parámetros es loaddata, entonces cargamos los datos
if [[ "$@" =~ "loaddata" ]]; then
    loaddata "$CONTRACT"
fi
