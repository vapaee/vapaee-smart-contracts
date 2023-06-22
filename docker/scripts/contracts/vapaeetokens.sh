#!/bin/bash
CONTRACT="vapaeetokens"

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

## -- load data --
function init() {
    ## Let's create the list of Vapaee Tokens
    print_title "--- Creating Vapaee Tokens ---"
    cleos_push_action vapaeetokens create '["tokenissuer", "21000000.000000 VPE"]' -p vapaeetokens
    cleos_push_action vapaeetokens create '["tokenissuer", "500000000.0000 CNT"]' -p vapaeetokens
    cleos_push_action vapaeetokens create '["tokenissuer", "50000000000.000000 TIPS"]' -p vapaeetokens
    cleos_push_action vapaeetokens create '["tokenissuer", "50000000000.000000 KOINE"]' -p vapaeetokens
    cleos_push_action vapaeetokens create '["tokenissuer", "5000000000.0000 EUROT"]' -p vapaeetokens
    cleos_push_action vapaeetokens create '["tokenissuer", "46116860184273800.00 MULITA"]' -p vapaeetokens
    cleos_push_action vapaeetokens create '["tokenissuer", "3000000000.000000 DIVERSE"]' -p vapaeetokens

    ## Let's issue and transfer some tokens
    print_title "--- Issuing and transfering Vapaee Tokens ---"
    cleos_push_action vapaeetokens issue '["tokenissuer", "1000000.000000 VPE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "ana", "100.000000 VPE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "bob", "100.000000 VPE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "tom", "100.000000 VPE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "kate", "100.000000 VPE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "vapaee", "10000.000000 VPE", ""]' -p tokenissuer

    cleos_push_action vapaeetokens issue '["tokenissuer", "10000000.0000 CNT", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "ana", "1000.0000 CNT", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "bob", "1000.0000 CNT", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "tom", "1000.0000 CNT", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "kate", "1000.0000 CNT", ""]' -p tokenissuer

    cleos_push_action vapaeetokens issue '["tokenissuer", "1000000000.000000 TIPS", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "ana", "100000.000000 TIPS", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "bob", "100000.000000 TIPS", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "tom", "100000.000000 TIPS", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "kate", "100000.000000 TIPS", ""]' -p tokenissuer

    cleos_push_action vapaeetokens issue '["tokenissuer", "1000000000.000000 KOINE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "ana", "1000.000000 KOINE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "bob", "1000.000000 KOINE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "tom", "1000.000000 KOINE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "kate", "1000.000000 KOINE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "vapaee", "1000000.000000 KOINE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "coinkoinonos", "1000000.000000 KOINE", ""]' -p tokenissuer

    cleos_push_action vapaeetokens issue '["tokenissuer", "100000000.0000 EUROT", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "ana", "1000.0000 EUROT", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "bob", "1000.0000 EUROT", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "tom", "1000.0000 EUROT", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "kate", "1000.0000 EUROT", ""]' -p tokenissuer

    cleos_push_action vapaeetokens issue '["tokenissuer", "100000000.00 MULITA", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "ana", "1000.00 MULITA", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "bob", "1000.00 MULITA", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "tom", "1000.00 MULITA", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "kate", "1000.00 MULITA", ""]' -p tokenissuer

    cleos_push_action vapaeetokens issue '["tokenissuer", "1000000000.000000 DIVERSE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "ana", "100000.000000 DIVERSE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "bob", "100000.000000 DIVERSE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "tom", "100000.000000 DIVERSE", ""]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "kate", "100000.000000 DIVERSE", ""]' -p tokenissuer

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
