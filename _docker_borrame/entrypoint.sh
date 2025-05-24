#!/bin/bash

# determine SCRIPT_DIR and import lib
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/scripts" && pwd)"
source $SCRIPT_DIR/_lib.sh

# execute command "cleos get info"
# if it return with error, print error and exit
# otherwise, print "Node is running"
command="cleos get info"
if ! $command; then
    echo -e "\e[31m\u2717 Nodeos is not running\e[0m"
    exit 1
else
    echo -e "\e[32m\u2714  Node is running\e[0m"
fi

# echo "--- import a private key for EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs ---"
# cleos wallet import --private-key 5JTR6k55fzRyDGNo861ruiek6c519n3eM7WtPFXgGPXV4orrqf5
# echo "--- create account for pruebavapaee ---"
# cleos create account eosio pruebavapaee EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs
# echo "--- compile contract ---"
# mkdir -p "$HOME/contracts/pruebavapaee/build/"
# eosio-cpp -abigen /app/contracts/pruebavapaee/pruebavapaee.cpp -o /app/contracts/pruebavapaee/build/pruebavapaee.wasm -I /app/include
# echo "--- deploy contract ---"
# cleos set contract pruebavapaee /app/contracts/pruebavapaee/build pruebavapaee.wasm pruebavapaee.abi -p pruebavapaee
# echo "--- call action init ---"
# cleos push action pruebavapaee init '[]' -p pruebavapaee
# echo "--- call action test ---"
# cleos push action pruebavapaee test '["a"]' -p pruebavapaee



# Ejecutar scripts
"$SCRIPT_DIR/accounts.sh" "$@"
"$SCRIPT_DIR/compile.sh" "$@"
"$SCRIPT_DIR/deploy.sh" "$@"
"$SCRIPT_DIR/init.sh" "$@"
"$SCRIPT_DIR/loaddata.sh" "$@"
echo "-------------------------------"
echo "$SCRIPT_DIR/accounts.sh"
echo "$SCRIPT_DIR/compare.sh"
echo "$SCRIPT_DIR/compile.sh"
echo "$SCRIPT_DIR/deploy.sh"
echo "$SCRIPT_DIR/init.sh"
echo "$SCRIPT_DIR/loaddata.sh"
echo "-------------------------------"
echo "$SCRIPT_DIR/compile.sh clear"
echo "-------------------------------"
echo "$SCRIPT_DIR/contracts/pruebavapaee.sh compile force deploy"
echo "$SCRIPT_DIR/contracts/basictoken.sh compile force deploy"
echo "$SCRIPT_DIR/contracts/acorntwitter.sh compile force deploy"
echo "$SCRIPT_DIR/contracts/telosbookdex.sh compile force deploy"
echo "$SCRIPT_DIR/contracts/telosmaindex.sh compile force deploy"
echo "$SCRIPT_DIR/contracts/telospooldex.sh compile force deploy"
echo "$SCRIPT_DIR/contracts/koinonospool.sh compile force deploy"
echo "$SCRIPT_DIR/contracts/vapaeepayhub.sh compile force deploy"
echo "$SCRIPT_DIR/contracts/vapaeetokens.sh compile force deploy"
echo "$SCRIPT_DIR/contracts/trngconsumer.sh compile force deploy"
echo "-------------------------------"
echo "telostest get table vapaeepayhub KOINE invoices"
echo "telostest get table vapaeepayhub vapaeepayhub payhubs"
echo "telostest get table vapaeepayhub vapaeepayhub payments"
echo "-------------------------------"
echo "Cuantos payments pendientes hay?" 
echo "  cleos get table vapaeepayhub vapaeepayhub payments -l 1000 | jq '.rows | length'"
echo "Realizar mantenimiento de los payments"
echo "  cleos push action vapaeepayhub update '["vpeworkerabc"]' -p vpeworkerabc@work"
echo "-------------------------------"



#telostest push action vapaeepayhub updatehub '["coinkoinonos", 4, [["0.5 PART", "Koinonos Staking Pools"], ["0.5 PART", "Koinonos Profits"]], ["KOINE", "EUROT"], "coinkoinonos"]' -p coinkoinonos
#telostest push action vapaeepayhub newname '["coinkoinonos", "Koinonos Profits"]' -p vapaeepayhub
#telostest push action vapaeepayhub newname '["vapaeeoracle", "Vapaée profits"]' -p vapaeepayhub
#telostest push action vapaeepayhub newpayhub '["coinkoinonos", "Koinonos Profits", [["1 PART", "coinkoinonos"]], ["KOINE", "EUROT"], "coinkoinonos"]' -p coinkoinonos
#telostest push action vapaeepayhub billing '["coinkoinonos", "main", "KOINE", "0.000000 KOINE", 0.001, "Koinonos Invoice"]' -p coinkoinonos
