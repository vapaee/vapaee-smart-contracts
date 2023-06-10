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
    echo -e "\e[32m\u2714 Node is running\e[0m"
fi

# Ejecutar scripts
"$SCRIPT_DIR/accounts.sh" "$@"
"$SCRIPT_DIR/compile.sh" "$@"
"$SCRIPT_DIR/deploy.sh" "$@"
"$SCRIPT_DIR/loaddata.sh" "$@"

echo "-------------------------------"
echo "$SCRIPT_DIR/accounts.sh"
echo "$SCRIPT_DIR/compile.sh"
echo "$SCRIPT_DIR/deploy.sh"
echo "$SCRIPT_DIR/loaddata.sh"
echo "-------------------------------"
echo "$SCRIPT_DIR/contracts/basictoken.sh compile deploy force"
echo "$SCRIPT_DIR/contracts/acorntwitter.sh compile deploy force"
echo "$SCRIPT_DIR/contracts/telosbookdex.sh compile deploy force"
echo "$SCRIPT_DIR/contracts/telosmaindex.sh compile deploy force"
echo "$SCRIPT_DIR/contracts/telospooldex.sh compile deploy force"
echo "$SCRIPT_DIR/contracts/vapaeepayhub.sh compile deploy force"
echo "$SCRIPT_DIR/contracts/vapaeetokens.sh compile deploy force"
echo "-------------------------------"
