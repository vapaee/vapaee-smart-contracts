#!/bin/bash
CONTRACT="vapaeepayhub"

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
    print_title "--- Vapaee PayHub Data ---"

    # ACTION newname(
    #     name owner,
    #     string vname
    # )

    print_subtitle "- Creating PayHub Names -"

    cleos_push_action vapaeepayhub newname '["vapaee", "Vapaee Payhub Fees"]' -p vapaeepayhub
    cleos_push_action vapaeepayhub newname '["coinkoinonos", "Koinonos Invoice"]' -p vapaeepayhub
    cleos_push_action vapaeepayhub newname '["coinkoinonos", "Koinonos Staking Pools"]' -p vapaeepayhub
    cleos_push_action vapaeepayhub newname '["acorntwitter", "Acorn Twitter"]' -p vapaeepayhub

    # ACTION stakeconfig(
    #     name action,
    #     name admin,
    #     symbol_code token,
    #     name contract,
    #     string title,
    #     std::vector<name> categories,
    #     string dayslock
    # )

    print_subtitle "- Creating Stake Configs -"

    cleos_push_action vapaeepayhub stakeconfig '["create", "coinkoinonos", "KOINE", "vapaeetokens", "Koinonos", ["a", "b", "c"], "0"]' -p coinkoinonos
    cleos_push_action vapaeepayhub stakeconfig '["create", "acorntwitter", "ACORN", "acornaccount", "Koinonos", ["a", "b"], "0"]' -p acorntwitter



    # ACTION stakepool(
    #     name action,
    #     symbol_code token,
    #     name poollabel,
    #     string title,
    #     string locktime
    # )

    print_subtitle "- Creating Stake pools -"

    cleos_push_action vapaeepayhub stakepool '["create", "KOINE", "long", "Koinonos Long Term", "10"]' -p coinkoinonos
    cleos_push_action vapaeepayhub stakepool '["create", "KOINE", "main", "Koinonos Main Pool", "0"]' -p coinkoinonos
    cleos_push_action vapaeepayhub stakepool '["create", "ACORN", "twitter", "Acorn Twitter", "0"]' -p acorntwitter


    

    # ACTION newpayhub(
    #     name admin,
    #     std::string vipname, 
    #     std::vector<std::tuple<asset,string>> recipients,
    #     std::vector<symbol_code> pockets,
    #     name billingto                
    # )

    print_subtitle "- Creating Payhub Nodes -"

    cleos_push_action vapaeepayhub newpayhub '["vapaee", "Vapaee Payhub Fees", [["1 PART", "vapaee"]], ["TLOS"], "vapaee"]' -p vapaee
    cleos_push_action vapaeepayhub newpayhub '["coinkoinonos", "Koinonos Staking Pools", [["0.25 PART", "KOINE-main"], ["0.75 PART", "KOINE-long"]], ["KOINE"], "coinkoinonos"]' -p coinkoinonos
    cleos_push_action vapaeepayhub newpayhub '["coinkoinonos", "Koinonos Invoice", [["1 PART", "Koinonos Staking Pools"]], ["KOINE", "EUROT"], "coinkoinonos"]' -p coinkoinonos
    cleos_push_action vapaeepayhub newpayhub '["acorntwitter", "Acorn Twitter", [["1.00 PART", "ACORN-twitter"]], ["ACORN"], "acorntwitter"]' -p acorntwitter



    # ACTION newleakpool(
        # name admin,
        # uint64_t target,
        # symbol_code token,
        # string title,
        # asset liquidity,
        # asset issueallaw,
        # name easing,
        # uint32_t epochstart,
        # uint32_t epochend
    # )

    # calculate epochstart and epochend
    cleos_push_action telosmaindex updatenow '[]' -p telosmaindex
    now=$(cleos get table telosmaindex telosmaindex state | jq -e '.rows[0].now')
    one_hour=3600
    epochstart=$(($now + 100))
    epochend=$(($epochstart + $one_hour))

    # get payhub ids
    id_koinonos_pools=$(cleos get table vapaeepayhub vapaeepayhub payhubs | jq -e '.rows[] | select(.alias=="Koinonos Staking Pools") | .id')
    id_acorntwitter=$(cleos get table vapaeepayhub vapaeepayhub payhubs | jq -e '.rows[] | select(.alias=="Acorn Twitter") | .id')

    print_subtitle "- Creating KOINE Leak Pool -"

    cleos_push_action vapaeetokens allowance '["vapaee", "vapaeepayhub", "1000000.000000 KOINE"]' -p vapaee
    cleos_push_action vapaeepayhub newleakpool '["vapaee", '$id_koinonos_pools', "KOINE", "Koinonos Reserve", "1000000.000000 KOINE", "0.000000 KOINE", "linear", '$epochstart', '$epochend']' -p vapaee

    print_subtitle "- Creating ACORN Leak Pool -"

    cleos_push_action acornaccount transfer '["acorntwitter", "vapaeetokens", "1000000.0000 ACORN", "allowance vapaeepayhub skip"]' -p acorntwitter
    cleos_push_action vapaeepayhub newleakpool '["acorntwitter", '$id_acorntwitter', "ACORN", "Acorn Reserve", "1000000.0000 ACORN", "0.0000 ACORN", "linear", '$epochstart', '$epochend']' -p acorntwitter

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
