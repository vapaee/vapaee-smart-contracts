#!/bin/bash

# determine SCRIPT_DIR and import lib
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source $SCRIPT_DIR/_lib.sh

# if clear is passed as first argument, delete all $HOME/contracts/*/build folders
if [[ "$1" == "clear" ]]; then
    print_title "Clearing all contracts..."
    rm -rf $HOME/contracts/*/build
else 
    print_section "--- Compiling all contracts ---"
    #"$SCRIPT_DIR/contracts/pruebavapaee.sh" "$@" compile
    "$SCRIPT_DIR/contracts/basictoken.sh" "$@" compile
    "$SCRIPT_DIR/contracts/koinonospool.sh" "$@" compile
    "$SCRIPT_DIR/contracts/telosbookdex.sh" "$@" compile
    "$SCRIPT_DIR/contracts/telosmaindex.sh" "$@" compile
    "$SCRIPT_DIR/contracts/telospooldex.sh" "$@" compile
    "$SCRIPT_DIR/contracts/vapaeepayhub.sh" "$@" compile
    "$SCRIPT_DIR/contracts/vapaeetokens.sh" "$@" compile
    "$SCRIPT_DIR/contracts/acorntwitter.sh" "$@" compile
fi
