#!/bin/bash

# determine SCRIPT_DIR and import lib
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source $SCRIPT_DIR/_lib.sh

# import a private key for EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs
cleos wallet import --private-key 5JTR6k55fzRyDGNo861ruiek6c519n3eM7WtPFXgGPXV4orrqf5

print_section "--- Creating Account names ---"

# Vapaee contracts 
contract_accounts=(
    "acorntwitter"
    "vapaeetokens"
    "koinonospool"
    "telosmaindex"
    "telosbookdex"
    "telospooldex"
    "telosnftsdex"
    "vapaeepayhub"
    "enhancednfts"
    "teloshistory"
)
for contract_account in "${contract_accounts[@]}"
do
  cleos_create_account eosio $contract_account EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs
  cleos_set_account_permission $contract_account EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs
done

# other contracts 
contract_accounts=(
    "eosio.token"
    "tokens.swaps"
    "acornaccount"
    "revelation21"
)
for contract_account in "${contract_accounts[@]}"
do
  cleos_create_account eosio $contract_account EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs
  cleos_set_account_permission $contract_account EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs
done

# auxiliary accounts
random_accounts=(
    "coinkoinonos"
    "montevideouy"
    "vapaee"
    "tokenissuer"
    "prueba"
    "pruebavapaee"
    "client.1"
    "client.2"
)
for random_account in "${random_accounts[@]}"
do
  cleos_create_account eosio $random_account EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs
done

# some random accounts
random_accounts=(
    "ana"
    "bob"
    "tom"
    "kate"
)
for random_account in "${random_accounts[@]}"
do
  cleos_create_account eosio $random_account EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs EOS5DGucugPz753AnMFfQtpzPeRq6prxrgP7WWAReWpuzChvE3sEs
done
