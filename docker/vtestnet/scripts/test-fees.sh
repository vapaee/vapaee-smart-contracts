#!/bin/bash
# rm /var/www/blockchain/eosio/data -fr




./1_create_accounts.sh

./2_deploy_contract.sh

./3_load_tokens.sh

./4_load_telos_decide.sh

./5_register_clients.sh

./6_deposits.sh

echo "pause..."
sleep 4
echo "alice sells -----------------------------"
cleos push action telosbookdex order '["alice", "sell", "100.00000000 CNT", "1.00000000 TLOS",0]' -p alice

sleep 2
echo "bob buys --------------------------------"
cleos push action telosbookdex order '["bob", "buy", "100.00000000 CNT", "1.00000000 TLOS",1]' -p bob
