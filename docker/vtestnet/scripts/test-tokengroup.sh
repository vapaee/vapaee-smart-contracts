#!/bin/bash
# rm /var/www/blockchain/eosio/data -fr




./1_create_accounts.sh

./2_deploy_contract.sh

./3_load_tokens.sh

./4_load_telos_decide.sh

./5_register_clients.sh

./6_deposits.sh


echo "- addtnkgroup -"
cleos push action telosbookdex addtnkgroup '["token.issuer", "Token Group 1", "website", "brief", "banner", "thumbnail"]' -p token.issuer

echo "- chnggroups BOX -"
cleos push action telosbookdex chnggroups '["BOX", [0,1]]' -p token.issuer@active

echo "- chnggroups CNT -"
cleos push action telosbookdex chnggroups '["CNT", [0,1]]' -p token.issuer@active

echo "trying to order BOX/CNT. DID IT FAIL ?? (IT SHOULD)"
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.10000000 CNT",0]' -p alice

echo "- setcurrency CNT -> currency -"
cleos push action telosbookdex setcurrency '["CNT", true, 1]' -p token.issuer@active

echo "- order BOX/CNT -"
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.10000000 CNT",0]' -p alice



# 