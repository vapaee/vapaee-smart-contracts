#!/bin/bash
CONTRACT="telosmaindex"

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
    #    ACTION addclient (
    #        name admin,
    #        name receiver,
    #        string params,
    #        string title,
    #        string website,
    #        string brief,
    #        string banner,
    #        string thumbnail
    #    )
    #

    print_title "--- Ading Clients to Telosmaindex ---"

    cleos_push_action telosmaindex addclient '["client.1", "client.1", "","Client 1", "http://client1.com", "client 1", "", ""]' -p "client.1"
    cleos_push_action telosmaindex addclient '["client.2", "client.2", "","Client 2", "http://client2.com", "client 2", "", ""]' -p "client.2"

    # ACTION addtoken (
    #     name contract,
    #     const symbol_code & symbol,
    #     uint8_t precision,
    #     name admin,
    #     string title,
    #     string website,
    #     string brief,
    #     string banner,
    #     string icon,
    #     string iconlg,
    #     string pcontact,
    #     string gcontact,
    #     bool tradeable,
    #     bool stable
    # )

    print_title "--- Registering token in Telosmaindex ---"

    print_subtitle "Registering TLOS"
    cleos_push_action eosio.token transfer '["tokenissuer", "telosmaindex", "100.0000 TLOS", "addtoken"]' -p tokenissuer
    cleos_push_action telosmaindex addtoken '["eosio.token", "TLOS", 4, "tokenissuer", "Telos", "https://telosfoundation.io", "A blockchain reimagining what is possible. Telos is faster, cheaper, greener, and honest. Beyond function, we are leading the way for a better Web 3.", "assets/uploads/telos-banner.jpg", "./assets/logos/telos.png", "./assets/logos/telos-lg.png", "https://telosfoundation.io", "https://t.me/HelloTelos", true, false]' -p tokenissuer

    print_subtitle "Registering VPE"
    cleos_push_action eosio.token transfer '["tokenissuer", "telosmaindex", "100.0000 TLOS", "addtoken"]' -p tokenissuer
    cleos_push_action telosmaindex addtoken '["vapaeetokens", "VPE", 6, "tokenissuer", "Vapaée", "https://vapaee.io", "Vapaée is a company that develops software for the EOSIO blockchain. It is a company that is dedicated to the development of software for the EOSIO blockchain. It is a company that is dedicated to the development of software for the EOSIO blockchain.", "assets/logos/vapaee.png", "assets/logos/vapaee-lg.png", "", "https://vapaee.io", "https://t.me/vapaee", true, false]' -p tokenissuer

    print_subtitle "Registering CNT"
    cleos_push_action eosio.token transfer '["tokenissuer", "telosmaindex", "100.0000 TLOS", "addtoken"]' -p tokenissuer
    cleos_push_action telosmaindex addtoken '["vapaeetokens", "CNT", 4, "tokenissuer", "Cards & Tokens", "http://cardsandtokens.com", "A platform where you can create themed albums and trading cards to collect and play making money in the process.", "assets/img/cards-and-tokens-1200x400.jpeg", "https://raw.githubusercontent.com/vapaee/vapaee.io-website/master/www/assets/logos/cnt.svg", "https://raw.githubusercontent.com/vapaee/vapaee.io-website/master/www/assets/logos/cnt-lg.svg", "https://vapaee.io", "https://t.me/vapaee", true, false]' -p tokenissuer

    print_subtitle "Registering TIPS"
    cleos_push_action eosio.token transfer '["tokenissuer", "telosmaindex", "100.0000 TLOS", "addtoken"]' -p tokenissuer
    cleos_push_action telosmaindex addtoken '["vapaeetokens", "TIPS", 6, "tokenissuer", "Tips", "https://koinonos.io/", "", "", "assets/logos/tips.png", "", "", "", true, false]' -p tokenissuer

    print_subtitle "Registering USDT"
    cleos_push_action eosio.token transfer '["tokenissuer", "telosmaindex", "100.0000 TLOS", "addtoken"]' -p tokenissuer
    cleos_push_action telosmaindex addtoken '["tokens.swaps", "USDT", 4, "tokenissuer", "Pegged USDT", "https://beta-bridge.tswaps.com/", "", "", "", "https://www.tswaps.com/tokens/usdt.png", "", "", true, true]' -p tokenissuer

    print_subtitle "Registering KOINE"
    cleos_push_action eosio.token transfer '["tokenissuer", "telosmaindex", "100.0000 TLOS", "addtoken"]' -p tokenissuer
    cleos_push_action telosmaindex addtoken '["vapaeetokens", "KOINE", 6, "tokenissuer", "Koinonos", "https://koinonos.io/", "", "", "", "assets/logos/koine.png", "", "", true, false]' -p tokenissuer

    print_subtitle "Registering ACORN"
    cleos_push_action eosio.token transfer '["tokenissuer", "telosmaindex", "100.0000 TLOS", "addtoken"]' -p tokenissuer
    cleos_push_action telosmaindex addtoken '["acornaccount", "ACORN", 4, "tokenissuer", "ACORN", "http://acorns.fun", "", "", "https://raw.githubusercontent.com/vapaee/vapaee.io-website/master/www/assets/logos/acorn.svg", "https://raw.githubusercontent.com/vapaee/vapaee.io-website/master/www/assets/logos/acorn-lg.png", "", "", true, false]' -p tokenissuer

    print_subtitle "Registering EUROT"
    cleos_push_action eosio.token transfer '["tokenissuer", "telosmaindex", "100.0000 TLOS", "addtoken"]' -p tokenissuer
    cleos_push_action telosmaindex addtoken '["vapaeetokens", "EUROT", 4, "tokenissuer", "EuroT", "https://koinonos.io/", "", "", "", "assets/logos/eurot.png", "", "", true, true]' -p tokenissuer

    print_subtitle "Registering HEART"
    cleos_push_action eosio.token transfer '["tokenissuer", "telosmaindex", "100.0000 TLOS", "addtoken"]' -p tokenissuer
    cleos_push_action telosmaindex addtoken '["revelation21", "HEART", 4, "tokenissuer", "HEART", "https://steemit.com/@steemchurch", "", "", "https://raw.githubusercontent.com/vapaee/vapaee.io-website/master/www/assets/logos/beautitude.png", "https://raw.githubusercontent.com/vapaee/vapaee.io-website/master/www/assets/logos/beautitude-lg.png", "", "", true, false]' -p tokenissuer

    print_subtitle "Registering MULITA"
    cleos_push_action eosio.token transfer '["tokenissuer", "telosmaindex", "100.0000 TLOS", "addtoken"]' -p tokenissuer
    cleos_push_action telosmaindex addtoken '["vapaeetokens", "MULITA", 2, "tokenissuer", "Crypto Mulita", "https://vapaee.io/dex/token/MULITA", "", "", "", "https://vapaee.io/assets/logos/mulita.png", "", "", true, true]' -p tokenissuer

    print_subtitle "Registering DIVERSE"
    cleos_push_action eosio.token transfer '["tokenissuer", "telosmaindex", "100.0000 TLOS", "addtoken"]' -p tokenissuer
    cleos_push_action telosmaindex addtoken '["vapaeetokens", "DIVERSE", 6, "tokenissuer", "DiversE", "https://koinonos.io/", "", "assets/logos/diverse-banner.png", "", "assets/logos/diverse.png", "", "", true, false]' -p tokenissuer


    print_title "--- Setting Currencies ---"

    cleos_push_action telosmaindex addcurrency '["EUROT", "https://koinonos.io/", "EuroT is the first stable token in parity with the Euro that connect your bank account directly to your Telos account. EuroT is also the main currency in the best native Telos DEX - vapaee.io/dex"]' -p tokenissuer
    

    
    # ACTION newmarket(
    #     const symbol_code & commodity,
    #     const symbol_code & currency,
    #     name converter
    # )

    print_title "--- Adding Markets ---"

    cleos_push_action telosmaindex newmarket '["TLOS", "EUROT", "telospooldex"]' -p telosmaindex
    cleos_push_action telosmaindex newmarket '["VPE", "EUROT", "telospooldex"]' -p telosmaindex
    cleos_push_action telosmaindex newmarket '["CNT", "EUROT", "telospooldex"]' -p telosmaindex
    cleos_push_action telosmaindex newmarket '["TIPS", "EUROT", "telospooldex"]' -p telosmaindex
    cleos_push_action telosmaindex newmarket '["USDT", "EUROT", "telospooldex"]' -p telosmaindex
    cleos_push_action telosmaindex newmarket '["ACORN", "EUROT", "telospooldex"]' -p telosmaindex
    cleos_push_action telosmaindex newmarket '["HEART", "EUROT", "telospooldex"]' -p telosmaindex
    cleos_push_action telosmaindex newmarket '["MULITA", "EUROT", "telospooldex"]' -p telosmaindex
    cleos_push_action telosmaindex newmarket '["DIVERSE", "EUROT", "telospooldex"]' -p telosmaindex

    cleos_push_action telosmaindex newmarket '["KOINE", "EUROT", "koinonospool"]' -p telosmaindex
    
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
