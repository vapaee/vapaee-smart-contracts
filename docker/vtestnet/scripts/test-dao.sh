#!/bin/bash
# rm /var/www/blockchain/eosio/data -fr




./1_create_accounts.sh

./2_deploy_contract.sh

./3_load_tokens.sh

./4_load_telos_decide.sh

./5_register_clients.sh

./6_deposits.sh

./7_orders.sh


echo "-- bob sells FAKE --"
cleos push action telosbookdex order '["bob", "sell", "5.00000000 FAKE", "0.10000000 TLOS",0]' -p bob
cleos push action telosbookdex order '["bob", "sell", "5.00000000 FAKE", "0.11000000 TLOS",0]' -p bob
cleos push action telosbookdex order '["bob", "sell", "5.00000000 FAKE", "0.12000000 TLOS",0]' -p bob
cleos push action telosbookdex order '["bob", "sell", "5.00000000 FAKE", "0.13000000 TLOS",0]' -p bob
cleos push action telosbookdex order '["bob", "sell", "5.00000000 FAKE", "0.14000000 TLOS",0]' -p bob

echo "create ballots over tokens -----------------------------"
cleos push action telosbookdex balloton '["bantoken", ["ROBO","proxibotstkn"], "bantoken ROBO proxibotstkn", "alice"]' -p alice
cleos push action telosbookdex balloton '["bantoken", ["FAKE","cryptomulita"], "bantoken FAKE cryptomulita", "alice"]' -p alice
cleos push action telosbookdex balloton '["savetoken", ["CNT","vapaeetokens"], "savetoken CNT vapaeetokens", "alice"]' -p alice
cleos push action telosbookdex balloton '["setcurrency", ["MULITA","cryptomulita"], "setcurrency MULITA cryptomulita", "alice"]' -p alice

echo "create ballots over fees -------------------------------"
cleos push action telosbookdex balloton '["makerfee", ["0.01010101 FEE"], "makerfee - sets a fee for the market creators", "alice"]' -p alice
cleos push action telosbookdex balloton '["takerfee", ["0.02020202 FEE"], "takerfee - sets a fee for the market orders takers (should be higher)", "alice"]' -p alice

echo "create prunnng ----------------------"
cleos push action telosbookdex balloton '["historyprune", ["111"], "historyprune - sets days of expiration for history", "alice"]' -p alice
cleos push action telosbookdex balloton '["hblockprune", ["222"], "hblockprune - sets days of expiration for history", "alice"]' -p alice
cleos push action telosbookdex balloton '["ballotsprune", ["22"], "ballotsprune - sets max count for ballots", "alice"]' -p alice
cleos push action telosbookdex balloton '["eventsprune", ["7"], "eventsprune - sets days of expiration for events", "alice"]' -p alice
cleos push action telosbookdex balloton '["pointsprune", ["2"], "pointsprune - sets weeks of expiration for user points", "alice"]' -p alice

echo "create ballots over token registration cost ------------"
cleos push action telosbookdex balloton '["regcost", ["333.0000 TLOS"], "regcost - sets the cost for registerin a new token in the DEX list", "alice"]' -p alice

echo "create ballots over appruval minimum percent of participation ------------"
cleos push action telosbookdex balloton '["approvalmin", ["0.5"], "minimum percent of participation", "alice"]' -p alice

cleos get table telosbookdex telosbookdex ballots
sleep 2

echo "all FAIL (repeated) ----------------------------"
cleos push action telosbookdex balloton '["bantoken", ["ROBO","proxibotstkn"], "aaaaaa", "alice"]' -p alice
cleos push action telosbookdex balloton '["bantoken", ["FAKE","cryptomulita"], "aaaaaa", "alice"]' -p alice
cleos push action telosbookdex balloton '["savetoken", ["CNT","vapaeetokens"], "aaaaaa", "alice"]' -p alice
cleos push action telosbookdex balloton '["setcurrency", ["MULITA","cryptomulita"], "aaaaaa", "alice"]' -p alice
cleos push action telosbookdex balloton '["makerfee", ["0.01010101 FEE"], "aaaaaa", "alice"]' -p alice
cleos push action telosbookdex balloton '["takerfee", ["0.02020202 FEE"], "aaaaaa", "alice"]' -p alice
cleos push action telosbookdex balloton '["historyprune", ["111"], "aaaaaa", "alice"]' -p alice
cleos push action telosbookdex balloton '["hblockprune", ["222"], "aaaaaa", "alice"]' -p alice
cleos push action telosbookdex balloton '["regcost", ["333.0000 TLOS"], "aaaaaa", "alice"]' -p alice
sleep 1

echo "vottng ------------------------------------------------"
echo " - bantoken ROBO - "
cleos push action telos.decide castvote '["bob", "", ["no"]]' -p bob
cleos push action telos.decide castvote '["alice", "", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "", ["no"]]' -p tom
cleos push action telos.decide castvote '["kate", "", ["no"]]' -p kate

echo " - bantoken FAKE - "
cleos push action telos.decide castvote '["bob", "............1", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............1", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............1", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............1", ["yes"]]' -p kate

echo " - savetoken CNT - "
cleos push action telos.decide castvote '["bob", "............2", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............2", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............2", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............2", ["yes"]]' -p kate

echo " - setcurrency MULITA - "
cleos push action telos.decide castvote '["bob", "............3", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............3", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............3", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............3", ["yes"]]' -p kate

echo " - makerfee - "
cleos push action telos.decide castvote '["bob", "............4", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............4", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............4", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............4", ["yes"]]' -p kate

echo " - takerfee - "
cleos push action telos.decide castvote '["bob", "............5", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............5", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............5", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............5", ["yes"]]' -p kate

echo " - historyprune - "
cleos push action telos.decide castvote '["bob", "............a", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............a", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............a", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............a", ["yes"]]' -p kate

echo " - hblockprune - "
cleos push action telos.decide castvote '["bob", "............b", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............b", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............b", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............b", ["yes"]]' -p kate

echo " - ballotsprune - "
cleos push action telos.decide castvote '["bob", "............c", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............c", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............c", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............c", ["yes"]]' -p kate

echo " - eventsprune - "
cleos push action telos.decide castvote '["bob", "............d", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............d", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............d", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............d", ["yes"]]' -p kate

echo " - pointsprune - "
cleos push action telos.decide castvote '["bob", "............e", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............e", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............e", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............e", ["yes"]]' -p kate

echo " - regcost - "
cleos push action telos.decide castvote '["bob", "............f", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............f", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............f", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............f", ["yes"]]' -p kate

echo " - approvalmin - "
cleos push action telos.decide castvote '["bob", "............g", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............g", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............g", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............g", ["yes"]]' -p kate


echo "waiting 5 sec for voting timeout..."
sleep 6

cleos get table telosbookdex telosbookdex state

echo "closing voting on telos.decide to generate broadcast"
cleos push action telos.decide closevoting '["", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............1", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............2", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............3", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............4", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............5", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............a", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............b", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............c", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............d", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............e", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............f", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............g", true]' -p telosbookdex@active



cleos get table telosbookdex telosbookdex state


echo "- testing a not approved ballot because of not reaching the minumum percent of participation"
cleos push action telosbookdex balloton '["regcost", ["111.1111 TLOS"], "aaaaaa", "alice"]' -p alice
cleos push action telosbookdex balloton '["setcurrency", ["MULITA","cryptomulita"], "setcurrency MULITA cryptomulita", "alice"]' -p alice
echo "  just 1 vote (less than 25%)"
cleos push action telos.decide castvote '["bob", "............h", ["yes"]]' -p bob

echo " - approvalmin - "
cleos push action telos.decide castvote '["bob", "............i", ["no"]]' -p bob
cleos push action telos.decide castvote '["alice", "............i", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............i", ["no"]]' -p tom
cleos push action telos.decide castvote '["kate", "............i", ["no"]]' -p kate

echo "  waiting another 5 sec for voting timeout..."
sleep 6
cleos push action telos.decide closevoting '["............h", true]' -p telosbookdex@active

exit 0
cleos push action telos.decide closevoting '["............i", true]' -p telosbookdex@active



cleos get table telosbookdex telosbookdex state



echo "--------- FIN ----------"

