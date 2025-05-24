#!/bin/bash
CONTRACT="trngconsumer"


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
    # requestrand(uint64_t seed, const name& caller, uint64_t min, uint64_t max)
    cleos_push_action trngconsumer requestrand '[123, "ana", 0, 10000]' -p ana
    cleos_push_action trngconsumer requestrand '[321, "ana", 100, 200]' -p ana
    echo "FUNCIONO ?????"
    sleep 2
}

# si alguno de los parámetros es init, entonces cargamos los datos
if [[ "$@" =~ "init" ]]; then
    init "$CONTRACT"
fi


## -- load data --
function loaddata() {
    CONTRACT="$1"
    print_title "--- Loading data into $CONTRACT ---"
}

# si alguno de los parámetros es loaddata, entonces cargamos los datos
if [[ "$@" =~ "loaddata" ]]; then
    loaddata "$CONTRACT"
fi
