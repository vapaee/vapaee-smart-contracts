#!/bin/bash

# determine SCRIPT_DIR and import lib
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source $SCRIPT_DIR/_lib.sh

print_section "--- Loading Data ---"

#"$SCRIPT_DIR/contracts/pruebavapaee.sh" "$@" loaddata
"$SCRIPT_DIR/contracts/basictoken.sh" "$@" loaddata
"$SCRIPT_DIR/contracts/vapaeetokens.sh" "$@" loaddata
"$SCRIPT_DIR/contracts/telosmaindex.sh" "$@" loaddata
"$SCRIPT_DIR/contracts/telosbookdex.sh" "$@" loaddata
"$SCRIPT_DIR/contracts/vapaeepayhub.sh" "$@" loaddata
"$SCRIPT_DIR/contracts/telospooldex.sh" "$@" loaddata
#"$SCRIPT_DIR/contracts/koinonospool.sh" "$@" loaddata
"$SCRIPT_DIR/contracts/acorntwitter.sh" "$@" loaddata
