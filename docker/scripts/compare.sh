#!/bin/bash

# determine SCRIPT_DIR and import lib
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source $SCRIPT_DIR/_lib.sh

print_section "--- Comparing local contracts with on chain contracts ABI ---"

#"$SCRIPT_DIR/contracts/pruebavapaee.sh" "$@" loaddata
"$SCRIPT_DIR/contracts/basictoken.sh" "$@" compare
"$SCRIPT_DIR/contracts/vapaeetokens.sh" "$@" compare
"$SCRIPT_DIR/contracts/telosmaindex.sh" "$@" compare
"$SCRIPT_DIR/contracts/telosbookdex.sh" "$@" compare
"$SCRIPT_DIR/contracts/vapaeepayhub.sh" "$@" compare
"$SCRIPT_DIR/contracts/telospooldex.sh" "$@" compare
"$SCRIPT_DIR/contracts/koinonospool.sh" "$@" compare
"$SCRIPT_DIR/contracts/acorntwitter.sh" "$@" compare
"$SCRIPT_DIR/contracts/trngconsumer.sh" "$@" compare
