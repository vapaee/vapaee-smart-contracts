#!/bin/bash

# determine SCRIPT_DIR and import lib
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source $SCRIPT_DIR/_lib.sh

print_section "--- Initialization ---"

#"$SCRIPT_DIR/contracts/pruebavapaee.sh" "$@" init
"$SCRIPT_DIR/contracts/basictoken.sh" "$@" init
"$SCRIPT_DIR/contracts/vapaeetokens.sh" "$@" init
"$SCRIPT_DIR/contracts/telosmaindex.sh" "$@" init
"$SCRIPT_DIR/contracts/telosbookdex.sh" "$@" init
"$SCRIPT_DIR/contracts/telospooldex.sh" "$@" init
"$SCRIPT_DIR/contracts/koinonospool.sh" "$@" init
"$SCRIPT_DIR/contracts/vapaeepayhub.sh" "$@" init
"$SCRIPT_DIR/contracts/acorntwitter.sh" "$@" init
