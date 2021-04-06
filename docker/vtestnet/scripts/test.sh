#!/bin/bash
# rm /var/www/blockchain/eosio/data -fr




./1_create_accounts.sh

./2_deploy_contract.sh

#!/bin/bash
HOME=/var/www/telos-dex-contract

cd $HOME/libraries/telos-decide/tests/contracts/eosio.token/

echo "-------- eosio.token (TLOS) ---------"
cleos set contract eosio.token $PWD -p eosio.token@active
cleos push action eosio.token create '[ "telosbookdex", "1000000000.0000 TLOS"]' -p eosio.token@active
cleos push action eosio.token create '[ "telosbookdex", "1000000000.0000 TLOS"]' -p eosio.token@active
cleos push action eosio.token issue '["telosbookdex", "10000.0000 TLOS", "memo 1000 TLOS"]' -p telosbookdex@active
sleep 1
# cleos push action eosio.token transfer '["telosbookdex", "alice", "1000.0000 TLOS", "memo 1000 TLOS"]' -p telosbookdex@active
cleos push action eosio.token transfer '["telosbookdex", "alice", "1001.0000 TLOS", "memo 1001 TLOS"]' -p telosbookdex@active
cleos push action eosio.token transfer '["telosbookdex", "bob", "1000.0000 TLOS", "memo 1000 TLOS"]' -p telosbookdex@active
cleos push action eosio.token transfer '["telosbookdex", "tom", "2000.0000 TLOS", "memo 2000 TLOS"]' -p telosbookdex@active
cleos push action eosio.token transfer '["telosbookdex", "kate", "1000.0000 TLOS", "memo 1000 TLOS"]' -p telosbookdex@active
cleos push action eosio.token transfer '["telosbookdex", "eosio", "1000.0000 TLOS", "memo 1000 TLOS"]' -p telosbookdex@active



echo "----- INITIALIZATION ----"
cleos push action telosbookdex init '[]' -p telosbookdex@active
sleep 1
echo "- table: state -"
cleos get table telosbookdex telosbookdex state
echo "- table: tokengroups -"
cleos get table telosbookdex telosbookdex tokengroups
echo "- table: tokens -"
cleos get table telosbookdex telosbookdex tokens
sleep 1

