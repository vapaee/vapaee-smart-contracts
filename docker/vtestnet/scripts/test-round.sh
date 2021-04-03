#!/bin/bash
# rm /var/www/blockchain/eosio/data -fr




./1_create_accounts.sh

./2_deploy_contract.sh

./3_load_tokens.sh

./4_load_telos_decide.sh

./5_register_clients.sh

./6_deposits.sh


echo "alice sells -----------------------------"
cleos push action telosbookdex order '["alice", "sell", "10000.00000000 CNT", "0.00510000 TLOS", 0]' -p alice

echo "pause..."
sleep 2
echo "bob buys --------------------------------"
cleos push action telosbookdex order '["bob", "buy", "1000.00000000 CNT", "0.00510000 TLOS", 0]' -p bob




cleos get table telosbookdex 0 history
