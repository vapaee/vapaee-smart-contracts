#!/bin/bash

# determine SCRIPT_DIR and import lib
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source $SCRIPT_DIR/_lib.sh

print_section "--- Deploying Contracts ---"

#"$SCRIPT_DIR/contracts/pruebavapaee.sh" "$@" deploy
"$SCRIPT_DIR/contracts/basictoken.sh" "$@" deploy
"$SCRIPT_DIR/contracts/vapaeetokens.sh" "$@" deploy
"$SCRIPT_DIR/contracts/telosmaindex.sh" "$@" deploy
"$SCRIPT_DIR/contracts/telosbookdex.sh" "$@" deploy
"$SCRIPT_DIR/contracts/telospooldex.sh" "$@" deploy
"$SCRIPT_DIR/contracts/vapaeepayhub.sh" "$@" deploy
"$SCRIPT_DIR/contracts/acorntwitter.sh" "$@" deploy
