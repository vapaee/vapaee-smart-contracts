#!/bin/bash
CONTRACT="koinonospool"

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

    print_title "--- init $CONTRACT ---"

    cleos_push_action vapaeetokens transfer '["tokenissuer", "koinonospool", "1000.000000 KOINE", "fund,KOINE/EUROT,tokenissuer"]' -p tokenissuer
    cleos_push_action vapaeetokens transfer '["tokenissuer", "koinonospool", "1000.0000 EUROT", "fund,KOINE/EUROT,tokenissuer"]' -p tokenissuer

    print_subtitle "adding koinonospool as KOINE issuer"
    cleos_push_action vapaeetokens issuer '["add", "koinonospool", "0.0000 KOINE"]' -p tokenissuer

}

# si alguno de los parámetros es init, entonces cargamos los datos
if [[ "$@" =~ "init" ]]; then
    init "$CONTRACT"
fi


## -- load data --
function loaddata() {

    print_title "--- loaddata $CONTRACT ---"

    cleos_push_action vapaeetokens transfer '["bob", "koinonospool", "10.0000 EUROT", "openpool.v1,koinonospool/KOINE,0.000000 KOINE,bob,Vapaée SWAP (10.0000 EUROT)"]' -p bob    
    cleos_push_action vapaeetokens transfer '["bob", "koinonospool", "10.000000 KOINE", "openpool.v1,koinonospool/EUROT,0.0000 EUROT,bob,Vapaée SWAP (10.000000 KOINE)"]' -p bob    
}

# si alguno de los parámetros es loaddata, entonces cargamos los datos
if [[ "$@" =~ "loaddata" ]]; then
    loaddata "$CONTRACT"
fi
