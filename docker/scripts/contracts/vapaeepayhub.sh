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

    cleos_push_action vapaeetokens allowance '["coinkoinonos", "vapaeepayhub", "1000000.000000 KOINE"]' -p coinkoinonos
    cleos_push_action vapaeepayhub newleakpool '["coinkoinonos", '$id_koinonos_pools', "KOINE", "Koinonos Reserve", "1000000.000000 KOINE", "0.000000 KOINE", "linear", '$epochstart', '$epochend']' -p coinkoinonos

    print_subtitle "- Creating ACORN Leak Pool -"

    cleos_push_action acornaccount transfer '["acorntwitter", "vapaeetokens", "1000000.0000 ACORN", "allowance vapaeepayhub skip"]' -p acorntwitter
    cleos_push_action vapaeepayhub newleakpool '["acorntwitter", '$id_acorntwitter', "ACORN", "Acorn Reserve", "1000000.0000 ACORN", "0.0000 ACORN", "linear", '$epochstart', '$epochend']' -p acorntwitter

    # ACTION billing(
    #     name admin,
    #     name invname,
    #     symbol_code token,
    #     asset fixed,
    #     double percent,
    #     string payhub
    # )

    #print_subtitle "- Creating Billing Configs -"
    #cleos_push_action vapaeepayhub billing '["coinkoinonos", "main", "KOINE", "0.000000 KOINE", 0.0, "Koinonos Invoice"]' -p coinkoinonos

}

# si alguno de los parámetros es init, entonces cargamos los datos
if [[ "$@" =~ "init" ]]; then
    init "$CONTRACT"
fi

## -- load data --
function loaddata() {
    print_title "--- loaddata --"

    # ACTION stake(
    #     name owner,
    #     asset quantity,
    #     name poollabel
    # )

    print_subtitle "--- Loading Stakes ---"
    print_subtitle "- bob -"

    cleos_push_action vapaeetokens allowance '["bob", "vapaeepayhub", "10.000000 KOINE"]' -p bob
    cleos_push_action vapaeepayhub stake '["bob", "10.000000 KOINE", "main"]' -p bob

    cleos_push_action vapaeetokens allowance '["bob", "vapaeepayhub", "10.000000 KOINE"]' -p bob
    cleos_push_action vapaeepayhub stake '["bob", "10.000000 KOINE", "long"]' -p bob

    cleos_push_action acornaccount transfer '["bob", "vapaeetokens", "100.0000 ACORN", "allowance vapaeepayhub"]' -p bob
    cleos_push_action vapaeepayhub stake '["bob", "100.0000 ACORN", "twitter"]' -p bob

    print_subtitle "- kate -"

    cleos_push_action vapaeetokens allowance '["kate", "vapaeepayhub", "12.000000 KOINE"]' -p kate
    cleos_push_action vapaeepayhub stake '["kate", "12.000000 KOINE", "main"]' -p kate

    cleos_push_action vapaeetokens allowance '["kate", "vapaeepayhub", "22.000000 KOINE"]' -p kate
    cleos_push_action vapaeepayhub stake '["kate", "22.000000 KOINE", "long"]' -p kate

    cleos_push_action acornaccount transfer '["kate", "vapaeetokens", "202.0000 ACORN", "allowance vapaeepayhub"]' -p kate
    cleos_push_action vapaeepayhub stake '["kate", "202.0000 ACORN", "twitter"]' -p kate


    print_subtitle "--- Loading Stakes ---"
    cleos push transaction '{"actions":[{"account":"vapaeepayhub","name":"newpayhub","authorization":[{"actor":"montevideouy","permission":"active"}],"data":{"admin":"montevideouy","vipname":"montevideouy","recipients":[{"field_0":"1 PART","field_1":"montevideouy"}],"pockets":["EUROT"],"billingto":"montevideouy"}}]}'



    print_subtitle "--- invoice ---"

    cleos_push_action vapaeetokens transfer '["kate", "koinonospool", "10.0000 EUROT", "openpool.v1,koinonospool/KOINE,0.000000 KOINE,vapaeepayhub,invoice montevideouy 10.0000 EUR Tiendas Montevideo"]' -p kate
    cleos_push_action vapaeepayhub movepayment '["montevideouy", "KOINE", "kate"]' -p kate


}

# si alguno de los parámetros es loaddata, entonces cargamos los datos
if [[ "$@" =~ "loaddata" ]]; then
    loaddata "$CONTRACT"
fi
