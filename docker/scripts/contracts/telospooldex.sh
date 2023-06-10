#!/bin/bash
CONTRACT="telospooldex"

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

    print_title "--- Initial Pools funding - telospooldex ---"

    cleos_push_action eosio.token transfer '["tokenissuer", "telospooldex", "1000.0000 TLOS", "fund,TLOS/EUROT,tokenissuer"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.0000 EUROT", "fund,TLOS/EUROT,tokenissuer"]' -p tokenissuer

    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.000000 VPE", "fund,VPE/EUROT,tokenissuer"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.0001 EUROT", "fund,TLOS/EUROT,tokenissuer"]' -p tokenissuer

    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.0000 CNT", "fund,CNT/EUROT,tokenissuer"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.0002 EUROT", "fund,TLOS/EUROT,tokenissuer"]' -p tokenissuer

    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.000000 TIPS", "fund,TIPS/EUROT,tokenissuer"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.0003 EUROT", "fund,TLOS/EUROT,tokenissuer"]' -p tokenissuer

    cleos_push_action tokens.swaps transfer '["tokenissuer", "telospooldex", "1000.0000 USDT", "fund,USDT/EUROT,tokenissuer"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.0004 EUROT", "fund,TLOS/EUROT,tokenissuer"]' -p tokenissuer

    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.000000 KOINE", "fund,KOINE/EUROT,tokenissuer"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.0005 EUROT", "fund,TLOS/EUROT,tokenissuer"]' -p tokenissuer

    cleos_push_action acornaccount transfer '["tokenissuer", "telospooldex", "1000.0000 ACORN", "fund,ACORN/EUROT,tokenissuer"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.0006 EUROT", "fund,TLOS/EUROT,tokenissuer"]' -p tokenissuer

    cleos_push_action revelation21 transfer '["tokenissuer", "telospooldex", "1000.0000 HEART", "fund,HEART/EUROT,tokenissuer"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.0007 EUROT", "fund,TLOS/EUROT,tokenissuer"]' -p tokenissuer

    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.00 MULITA", "fund,MULITA/EUROT,tokenissuer"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.0008 EUROT", "fund,TLOS/EUROT,tokenissuer"]' -p tokenissuer

    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.000000 DIVERSE", "fund,DIVERSE/EUROT,tokenissuer"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "telospooldex", "1000.0009 EUROT", "fund,TLOS/EUROT,tokenissuer"]' -p tokenissuer

}

# si alguno de los parámetros es init, entonces cargamos los datos
if [[ "$@" =~ "init" ]]; then
    init "$CONTRACT"
fi

## -- load data --
function loaddata() {
    print_title "--- loaddata --"
}

# si alguno de los parámetros es loaddata, entonces cargamos los datos
if [[ "$@" =~ "loaddata" ]]; then
    loaddata "$CONTRACT"
fi
