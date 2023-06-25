#!/bin/bash
CONTRACT="basictoken"


# determine SCRIPT_DIR and import lib
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../" && pwd)"
source $SCRIPT_DIR/_lib.sh

function clear() {
    CONTRACT="$1"
    print_title "clearing $CONTRACT..."
    rm -rf "$HOME/contracts/$CONTRACT/build"
}

# si alguno de los parámetros es clear, entonces compilamos
if [[ "$@" =~ "clear" ]]; then
    clear "$CONTRACT"
fi


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
    local contract=vapaeetokens
    local contract_dir="$HOME/contracts/$contract/build"
    local contract_wasm="$contract.wasm"
    local contract_abi="$contract.abi"

    print_title "--- Deploying Basic Tokens ---"

    local command="cleos set contract eosio.token $contract_dir $contract_wasm $contract_abi -p eosio.token"
    execute_command "$command" 2
    cleos_push_action eosio.token init '[]' -p eosio.token

    local command="cleos set contract tokens.swaps $contract_dir $contract_wasm $contract_abi -p tokens.swaps"
    execute_command "$command" 2
    cleos_push_action tokens.swaps init '[]' -p tokens.swaps

    local command="cleos set contract acornaccount $contract_dir $contract_wasm $contract_abi -p acornaccount"
    execute_command "$command" 2
    cleos_push_action acornaccount init '[]' -p acornaccount

    local command="cleos set contract revelation21 $contract_dir $contract_wasm $contract_abi -p revelation21"
    execute_command "$command" 2
    cleos_push_action revelation21 init '[]' -p revelation21
}

# si alguno de los parámetros es deploy, entonces deployamos
if [[ "$@" =~ "deploy" ]]; then
    deploy "$CONTRACT"
fi


## -- init --
function init() {

    print_title "--- Creating Basic Tokens ---"
    cleos_push_action eosio.token create '["tokenissuer", "1000000000.0000 TLOS"]' -p tokenissuer
    cleos_push_action tokens.swaps create '["tokenissuer", "1000000000000.0000 USDT"]' -p tokenissuer
    cleos_push_action acornaccount create '["tokenissuer", "461168601842738.0000 ACORN"]' -p tokenissuer
    cleos_push_action revelation21 create '["tokenissuer", "100000000.0000 HEART"]' -p tokenissuer

    print_title "--- Issuing and transfering Basic Tokens ---"
    cleos_push_action eosio.token issue '["tokenissuer", "1000000000.0000 TLOS", ""]' -p tokenissuer
    cleos_push_action eosio.token transfer '["tokenissuer", "ana", "100.0000 TLOS", ""]' -p tokenissuer
    cleos_push_action eosio.token transfer '["tokenissuer", "bob", "100.0000 TLOS", ""]' -p tokenissuer
    cleos_push_action eosio.token transfer '["tokenissuer", "tom", "100.0000 TLOS", ""]' -p tokenissuer
    cleos_push_action eosio.token transfer '["tokenissuer", "kate", "100.0000 TLOS", ""]' -p tokenissuer
    cleos_push_action eosio.token transfer '["tokenissuer", "montevideouy", "1000.0000 TLOS", ""]' -p tokenissuer
    cleos_push_action eosio.token transfer '["tokenissuer", "coinkoinonos", "1000.0000 TLOS", ""]' -p tokenissuer

    cleos_push_action tokens.swaps issue '["tokenissuer", "1000000000000.0000 USDT", ""]' -p tokenissuer
    cleos_push_action tokens.swaps transfer '["tokenissuer", "ana", "1000.0000 USDT", ""]' -p tokenissuer
    cleos_push_action tokens.swaps transfer '["tokenissuer", "bob", "1000.0000 USDT", ""]' -p tokenissuer
    cleos_push_action tokens.swaps transfer '["tokenissuer", "tom", "1000.0000 USDT", ""]' -p tokenissuer
    cleos_push_action tokens.swaps transfer '["tokenissuer", "kate", "1000.0000 USDT", ""]' -p tokenissuer
    cleos_push_action tokens.swaps transfer '["tokenissuer", "montevideouy", "1000.0000 USDT", ""]' -p tokenissuer
    cleos_push_action tokens.swaps transfer '["tokenissuer", "coinkoinonos", "1000.0000 USDT", ""]' -p tokenissuer

    cleos_push_action acornaccount issue '["tokenissuer", "461168601842738.0000 ACORN", ""]' -p tokenissuer
    cleos_push_action acornaccount transfer '["tokenissuer", "ana", "10000.0000 ACORN", ""]' -p tokenissuer
    cleos_push_action acornaccount transfer '["tokenissuer", "bob", "10000.0000 ACORN", ""]' -p tokenissuer
    cleos_push_action acornaccount transfer '["tokenissuer", "tom", "10000.0000 ACORN", ""]' -p tokenissuer
    cleos_push_action acornaccount transfer '["tokenissuer", "kate", "10000.0000 ACORN", ""]' -p tokenissuer
    cleos_push_action acornaccount transfer '["tokenissuer", "montevideouy", "10000.0000 ACORN", ""]' -p tokenissuer
    cleos_push_action acornaccount transfer '["tokenissuer", "coinkoinonos", "10000.0000 ACORN", ""]' -p tokenissuer
    cleos_push_action acornaccount transfer '["tokenissuer", "acorntwitter", "1000000.0000 ACORN", "skip"]' -p tokenissuer

    cleos_push_action revelation21 issue '["tokenissuer", "100000000.0000 HEART", ""]' -p tokenissuer
    cleos_push_action revelation21 transfer '["tokenissuer", "ana", "1000.0000 HEART", ""]' -p tokenissuer
    cleos_push_action revelation21 transfer '["tokenissuer", "bob", "1000.0000 HEART", ""]' -p tokenissuer
    cleos_push_action revelation21 transfer '["tokenissuer", "tom", "1000.0000 HEART", ""]' -p tokenissuer
    cleos_push_action revelation21 transfer '["tokenissuer", "kate", "1000.0000 HEART", ""]' -p tokenissuer
    cleos_push_action revelation21 transfer '["tokenissuer", "montevideouy", "1000.0000 HEART", ""]' -p tokenissuer
    cleos_push_action revelation21 transfer '["tokenissuer", "coinkoinonos", "1000.0000 HEART", ""]' -p tokenissuer



    #cleos_push_action eosio.token init '[]' -p tokenissuer
}

# si alguno de los parámetros es init, entonces cargamos los datos
if [[ "$@" =~ "init" ]]; then
    init "$CONTRACT"
fi


## -- load data --
function loaddata() {

    print_title "- loaddata $1 -"
    #cleos_push_action eosio.token transfer '["tokenissuer", "vapaeetokens", "1.0000 TLOS", "deposit"]' -p tokenissuer
    #cleos_push_action acornaccount transfer '["tokenissuer", "vapaeetokens", "1.0000 ACORN", "deposit"]' -p tokenissuer
    #cleos_push_action tokens.swaps transfer '["tokenissuer", "vapaeetokens", "1.0000 USDT", "deposit"]' -p tokenissuer
    #cleos_push_action revelation21 transfer '["tokenissuer", "vapaeetokens", "1.0000 HEART", "deposit"]' -p tokenissuer

}

# si alguno de los parámetros es loaddata, entonces cargamos los datos
if [[ "$@" =~ "loaddata" ]]; then
    loaddata "$CONTRACT"
fi
