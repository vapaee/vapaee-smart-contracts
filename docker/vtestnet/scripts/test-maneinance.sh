#!/bin/bash
# rm /var/www/blockchain/eosio/data -fr




./1_create_accounts.sh

./2_deploy_contract.sh

./3_load_tokens.sh

./4_load_telos_decide.sh

cleos push action telosbookdex timeoffset '[186500]' -p telosbookdex

./5_register_clients.sh

./6_deposits.sh

./7_orders.sh




# 86400
echo "-- bob sells CNT --"
cleos push action telosbookdex timeoffset '[186360]' -p telosbookdex
cleos push action telosbookdex order '["bob", "sell", "5.00000000 CNT", "0.10000000 TLOS",0]' -p bob
cleos push action telosbookdex timeoffset '[186350]' -p telosbookdex
cleos push action telosbookdex order '["bob", "sell", "5.00000000 CNT", "0.11000000 TLOS",0]' -p bob
cleos push action telosbookdex timeoffset '[186340]' -p telosbookdex
cleos push action telosbookdex order '["bob", "sell", "5.00000000 CNT", "0.12000000 TLOS",0]' -p bob
cleos push action telosbookdex timeoffset '[186330]' -p telosbookdex
cleos push action telosbookdex order '["bob", "sell", "5.00000000 CNT", "0.13000000 TLOS",0]' -p bob
cleos push action telosbookdex timeoffset '[186320]' -p telosbookdex
cleos push action telosbookdex order '["bob", "sell", "5.00000000 CNT", "0.14000000 TLOS",0]' -p bob


echo "-- bob sells FAKE --"
cleos push action telosbookdex timeoffset '[186460]' -p telosbookdex
cleos push action telosbookdex order '["bob", "sell", "5.00000000 FAKE", "0.10000000 TLOS",0]' -p bob
cleos push action telosbookdex timeoffset '[186450]' -p telosbookdex
cleos push action telosbookdex order '["bob", "sell", "5.00000000 FAKE", "0.11000000 TLOS",0]' -p bob
cleos push action telosbookdex timeoffset '[186440]' -p telosbookdex
cleos push action telosbookdex order '["bob", "sell", "5.00000000 FAKE", "0.12000000 TLOS",0]' -p bob
cleos push action telosbookdex timeoffset '[186430]' -p telosbookdex
cleos push action telosbookdex order '["bob", "sell", "5.00000000 FAKE", "0.13000000 TLOS",0]' -p bob
cleos push action telosbookdex timeoffset '[186420]' -p telosbookdex
cleos push action telosbookdex order '["bob", "sell", "5.00000000 FAKE", "0.14000000 TLOS",0]' -p bob
cleos push action telosbookdex timeoffset '[0]' -p telosbookdex

echo "create ballots over history prune ----------------------"
cleos push action telosbookdex balloton '["historyprune", ["1"], "historyprune - sets days of expiration", "alice"]' -p alice
cleos push action telosbookdex balloton '["hblockprune", ["1"], "hblockprune - sets days of expiration", "alice"]' -p alice
cleos push action telosbookdex balloton '["ballotsprune", ["0"], "ballotsprune - sets max count for ballots", "alice"]' -p alice
cleos push action telosbookdex balloton '["eventsprune", ["1"], "eventsprune - sets days of expiration for events", "alice"]' -p alice
cleos push action telosbookdex balloton '["pointsprune", ["1"], "pointsprune - sets weeks of expiration for points", "alice"]' -p alice

echo "create ballots over tokens -----------------------------"
cleos push action telosbookdex balloton '["bantoken", ["FAKE","cryptomulita"], "bantoken FAKE cryptomulita", "alice"]' -p alice
cleos push action telosbookdex balloton '["savetoken", ["CNT","vapaeetokens"], "savetoken CNT vapaeetokens", "alice"]' -p alice
cleos push action telosbookdex balloton '["setcurrency", ["MULITA","cryptomulita"], "setcurrency MULITA cryptomulita", "alice"]' -p alice

echo "vottng ------------------------------------------------"
cleos push action telos.decide castvote '["bob", "", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "", ["yes"]]' -p alice
cleos push action telos.decide castvote '["tom", "", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "", ["yes"]]' -p kate
cleos push action telos.decide castvote '["bob", "............1", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............1", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............1", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............1", ["yes"]]' -p kate
cleos push action telos.decide castvote '["bob", "............2", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............2", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............2", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............2", ["yes"]]' -p kate
cleos push action telos.decide castvote '["bob", "............3", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............3", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............3", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............3", ["yes"]]' -p kate
cleos push action telos.decide castvote '["bob", "............4", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............4", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............4", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............4", ["yes"]]' -p kate
cleos push action telos.decide castvote '["bob", "............5", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............5", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............5", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............5", ["yes"]]' -p kate
cleos push action telos.decide castvote '["bob", "............a", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............a", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............a", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............a", ["yes"]]' -p kate
cleos push action telos.decide castvote '["bob", "............b", ["yes"]]' -p bob
cleos push action telos.decide castvote '["alice", "............b", ["no"]]' -p alice
cleos push action telos.decide castvote '["tom", "............b", ["yes"]]' -p tom
cleos push action telos.decide castvote '["kate", "............b", ["yes"]]' -p kate

echo "closing voting on telos.decide to generate broadcast --"
sleep 6
cleos push action telos.decide closevoting '["", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............1", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............2", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............3", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............4", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............5", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............a", true]' -p telosbookdex@active
cleos push action telos.decide closevoting '["............b", true]' -p telosbookdex@active

cleos get table telosbookdex telosbookdex state
sleep 1

cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5
cleos push action telosbookdex maintenance '["kate"]' -p kate && sleep 0.5

cleos get table telosbookdex telosbookdex historyall
sleep 1

echo "-- alice sells EDNA --"
cleos push action telosbookdex order '["tom", "sell", "5.00000000 EDNA", "0.40000000 TLOS",0]' -p tom
cleos push action telosbookdex order '["tom", "sell", "5.00000000 EDNA", "0.41000000 TLOS",0]' -p tom
cleos push action telosbookdex order '["tom", "sell", "5.00000000 EDNA", "0.42000000 TLOS",0]' -p tom
cleos push action telosbookdex order '["tom", "sell", "5.00000000 EDNA", "0.43000000 TLOS",0]' -p tom
cleos push action telosbookdex order '["tom", "sell", "5.00000000 EDNA", "0.44000000 TLOS",0]' -p tom
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.45000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.46000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.47000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.48000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.49000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.50000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.51000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.52000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.53000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.54000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.55000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.56000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.57000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.58000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 EDNA", "0.59000000 TLOS",0]' -p alice

echo "-- alice buys EDNA --"
cleos push action telosbookdex order '["kate", "buy", "5.00000000 EDNA", "0.10000000 TLOS",0]' -p kate
cleos push action telosbookdex order '["kate", "buy", "5.00000000 EDNA", "0.11000000 TLOS",0]' -p kate
cleos push action telosbookdex order '["kate", "buy", "5.00000000 EDNA", "0.12000000 TLOS",0]' -p kate
cleos push action telosbookdex order '["kate", "buy", "5.00000000 EDNA", "0.13000000 TLOS",0]' -p kate
cleos push action telosbookdex order '["kate", "buy", "5.00000000 EDNA", "0.14000000 TLOS",0]' -p kate
cleos push action telosbookdex order '["kate", "buy", "5.00000000 EDNA", "0.15000000 TLOS",0]' -p kate
cleos push action telosbookdex order '["kate", "buy", "5.00000000 EDNA", "0.16000000 TLOS",0]' -p kate
cleos push action telosbookdex order '["alice", "buy", "5.00000000 EDNA", "0.17000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 EDNA", "0.18000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 EDNA", "0.19000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 EDNA", "0.20000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 EDNA", "0.21000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 EDNA", "0.22000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 EDNA", "0.23000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 EDNA", "0.24000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 EDNA", "0.25000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 EDNA", "0.26000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 EDNA", "0.27000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 EDNA", "0.28000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 EDNA", "0.29000000 TLOS",0]' -p alice

cleos push action telosbookdex order '["bob", "buy", "5.00000000 EDNA", "0.40000000 TLOS",0]' -p bob
cleos push action telosbookdex order '["bob", "buy", "5.00000000 EDNA", "0.41000000 TLOS",0]' -p bob
cleos push action telosbookdex order '["bob", "buy", "5.00000000 EDNA", "0.42000000 TLOS",0]' -p bob
cleos push action telosbookdex order '["bob", "buy", "5.00000000 EDNA", "0.43000000 TLOS",0]' -p bob
cleos push action telosbookdex order '["bob", "buy", "5.00000000 EDNA", "0.44000000 TLOS",0]' -p bob

# cleos get table telosbookdex telosbookdex historyall
# sleep 1
# cleos push action telosbookdex maintenance '["alice"]' -p alice
# cleos get table telosbookdex telosbookdex historyall
# sleep 1
# cleos push action telosbookdex maintenance '["alice"]' -p alice
# cleos get table telosbookdex telosbookdex historyall
# sleep 1
# cleos push action telosbookdex maintenance '["alice"]' -p alice
# cleos get table telosbookdex telosbookdex historyall
# sleep 1
# cleos push action telosbookdex maintenance '["alice"]' -p alice
# cleos get table telosbookdex telosbookdex historyall
# sleep 1
# cleos push action telosbookdex maintenance '["alice"]' -p alice
# cleos get table telosbookdex telosbookdex historyall
# sleep 1
# cleos push action telosbookdex maintenance '["alice"]' -p alice
# cleos get table telosbookdex telosbookdex historyall
# sleep 1
# 
# 
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# cleos push action telosbookdex maintenance '["alice"]' -p alice && sleep 0.5
# 