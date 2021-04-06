#!/bin/bash

echo "-- 6_deposits --"
cleos push action eosio.token transfer  '["bob",  "telosbookdex","100.0000 TLOS","deposit"]' -p bob@active
cleos push action eosio.token transfer  '["alice","telosbookdex","1000.0000 TLOS","deposit"]' -p alice@active
cleos push action eosio.token transfer  '["kate", "telosbookdex","100.0000 TLOS","deposit"]' -p kate@active
cleos push action eosio.token transfer  '["tom",  "telosbookdex","1000.0000 TLOS","deposit"]' -p tom@active

cleos push action acornaccount transfer  '["bob",  "telosbookdex","50.0000 ACORN","deposit"]' -p bob@active
cleos push action acornaccount transfer  '["alice","telosbookdex","1000.0000 ACORN","deposit"]' -p alice@active
cleos push action acornaccount transfer  '["tom",  "telosbookdex","50.0000 ACORN","deposit"]' -p tom@active
cleos push action acornaccount transfer  '["kate", "telosbookdex","50.0000 ACORN","deposit"]' -p kate@active

cleos push action futboltokens transfer  '["bob",  "telosbookdex","1000.0000 FUTBOL","deposit"]' -p bob@active
cleos push action futboltokens transfer  '["alice","telosbookdex","1000.0000 FUTBOL","deposit"]' -p alice@active
cleos push action futboltokens transfer  '["tom",  "telosbookdex","1000.0000 FUTBOL","deposit"]' -p tom@active
cleos push action futboltokens transfer  '["kate", "telosbookdex","1000.0000 FUTBOL","deposit"]' -p kate@active

cleos push action qubicletoken transfer  '["bob",  "telosbookdex","100.0000 QBE","deposit"]' -p bob@active
cleos push action qubicletoken transfer  '["alice","telosbookdex","100.0000 QBE","deposit"]' -p alice@active
cleos push action qubicletoken transfer  '["tom",  "telosbookdex","100.0000 QBE","deposit"]' -p tom@active
cleos push action qubicletoken transfer  '["kate", "telosbookdex","100.0000 QBE","deposit"]' -p kate@active

cleos push action stablecarbon transfer  '["bob",  "telosbookdex","1000.0000 TLOSD","deposit"]' -p bob@active
cleos push action stablecarbon transfer  '["alice","telosbookdex","1000.0000 TLOSD","deposit"]' -p alice@active
cleos push action stablecarbon transfer  '["tom",  "telosbookdex","1000.0000 TLOSD","deposit"]' -p tom@active
cleos push action stablecarbon transfer  '["kate", "telosbookdex","1000.0000 TLOSD","deposit"]' -p kate@active

cleos push action stablecoin.z transfer  '["bob",  "telosbookdex","1000.00 EZAR","deposit"]' -p bob@active
cleos push action stablecoin.z transfer  '["alice","telosbookdex","1000.00 EZAR","deposit"]' -p alice@active
cleos push action stablecoin.z transfer  '["tom",  "telosbookdex","1000.00 EZAR","deposit"]' -p tom@active
cleos push action stablecoin.z transfer  '["kate", "telosbookdex","1000.00 EZAR","deposit"]' -p kate@active

cleos push action ednazztokens transfer  '["bob",  "telosbookdex","100.0000 EDNA","deposit"]' -p bob@active
cleos push action ednazztokens transfer  '["alice","telosbookdex","100.0000 EDNA","deposit"]' -p alice@active
cleos push action ednazztokens transfer  '["tom",  "telosbookdex","100.0000 EDNA","deposit"]' -p tom@active
cleos push action ednazztokens transfer  '["kate", "telosbookdex","100.0000 EDNA","deposit"]' -p kate@active

cleos push action oliveaccount transfer  '["bob",  "telosbookdex","100.0000 OLIVE","deposit"]' -p bob@active
cleos push action oliveaccount transfer  '["alice","telosbookdex","100.0000 OLIVE","deposit"]' -p alice@active
cleos push action oliveaccount transfer  '["tom",  "telosbookdex","100.0000 OLIVE","deposit"]' -p tom@active
cleos push action oliveaccount transfer  '["kate", "telosbookdex","100.0000 OLIVE","deposit"]' -p kate@active

cleos push action proxibotstkn transfer  '["bob",  "telosbookdex","100.0000 ROBO","deposit"]' -p bob@active
cleos push action proxibotstkn transfer  '["alice","telosbookdex","100.0000 ROBO","deposit"]' -p alice@active
cleos push action proxibotstkn transfer  '["tom",  "telosbookdex","100.0000 ROBO","deposit"]' -p tom@active
cleos push action proxibotstkn transfer  '["kate", "telosbookdex","100.0000 ROBO","deposit"]' -p kate@active

cleos push action viitasphere1 transfer  '["bob",  "telosbookdex","100.0000 VIITA","deposit"]' -p bob@active
cleos push action viitasphere1 transfer  '["alice","telosbookdex","100.0000 VIITA","deposit"]' -p alice@active
cleos push action viitasphere1 transfer  '["tom",  "telosbookdex","100.0000 VIITA","deposit"]' -p tom@active
cleos push action viitasphere1 transfer  '["kate", "telosbookdex","100.0000 VIITA","deposit"]' -p kate@active

cleos push action teachology14 transfer  '["bob",  "telosbookdex","100.0000 TEACH","deposit"]' -p bob@active
cleos push action teachology14 transfer  '["alice","telosbookdex","100.0000 TEACH","deposit"]' -p alice@active
cleos push action teachology14 transfer  '["tom",  "telosbookdex","100.0000 TEACH","deposit"]' -p tom@active
cleos push action teachology14 transfer  '["kate", "telosbookdex","100.0000 TEACH","deposit"]' -p kate@active

cleos push action telosdacdrop transfer '["bob",  "telosbookdex","50000.0000 TLOSDAC","deposit"]' -p bob@active
cleos push action telosdacdrop transfer '["alice","telosbookdex","50000.0000 TLOSDAC","deposit"]' -p alice@active
cleos push action telosdacdrop transfer '["tom",  "telosbookdex","50000.0000 TLOSDAC","deposit"]' -p tom@active
cleos push action telosdacdrop transfer '["kate", "telosbookdex","50000.0000 TLOSDAC","deposit"]' -p kate@active

cleos push action vapaeetokens transfer '["bob",  "telosbookdex","10000.0000 CNT","deposit"]' -p bob@active
cleos push action vapaeetokens transfer '["alice","telosbookdex","10000.0000 CNT","deposit"]' -p alice@active
cleos push action vapaeetokens transfer '["kate", "telosbookdex","10000.0000 CNT","deposit"]' -p kate@active

cleos push action vapaeetokens transfer  '["bob",  "telosbookdex","1000.0000 BOX","deposit"]' -p bob@active
cleos push action vapaeetokens transfer  '["alice","telosbookdex","1000.0000 BOX","deposit"]' -p alice@active


cleos push action cryptomulita transfer '["bob",  "telosbookdex","1000.0000 MULITA","deposit"]' -p bob@active
cleos push action cryptomulita transfer '["alice","telosbookdex","1000.0000 MULITA","deposit"]' -p alice@active
cleos push action cryptomulita transfer '["tom",  "telosbookdex","1000.0000 MULITA","deposit"]' -p tom@active
cleos push action cryptomulita transfer '["kate", "telosbookdex","1000.0000 MULITA","deposit"]' -p kate@active

cleos push action cryptomulita transfer '["bob",  "telosbookdex","1000.0000 FAKE","deposit"]' -p bob@active
cleos push action cryptomulita transfer '["alice","telosbookdex","1000.0000 FAKE","deposit"]' -p alice@active
cleos push action cryptomulita transfer '["tom",  "telosbookdex","1000.0000 FAKE","deposit"]' -p tom@active
cleos push action cryptomulita transfer '["kate", "telosbookdex","1000.0000 FAKE","deposit"]' -p kate@active

echo "-- 6_deposits finiched --"