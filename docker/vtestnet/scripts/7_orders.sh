#!/bin/bash
echo "-- 7_orders --"

echo "-- alice sells TLOS/TLOSD --"
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.04000000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.04100000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.04200000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.04300000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.04400000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.04500000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.04600000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.04700000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.04800000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.04900000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.05000000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.05100000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.05200000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.05300000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.05400000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.05500000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.05600000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.05700000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.05800000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 TLOS", "0.05900000 TLOSD",0]' -p alice

echo "-- alice buys TLOS/TLOSD --"
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.01000000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.01100000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.01200000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.01300000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.01400000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.01500000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.01600000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.01700000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.01800000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.01900000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.02000000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.02100000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.02200000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.02300000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.02400000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.02500000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.02600000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.02700000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.02800000 TLOSD",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 TLOS", "0.02900000 TLOSD",0]' -p alice

echo "-- alice sells ACORN --"
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.40000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.41000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.42000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.43000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.44000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.45000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.46000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.47000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.48000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.49000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.50000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.51000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.52000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.53000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.54000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.55000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.56000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.57000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.58000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 ACORN", "0.59000000 TLOS",1]' -p alice

echo "-- alice buys ACORN --"
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.10000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.11000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.12000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.13000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.14000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.15000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.16000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.17000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.18000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.19000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.20000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.21000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.22000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.23000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.24000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.25000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.26000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.27000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.28000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 ACORN", "0.29000000 TLOS",1]' -p alice

cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.40000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.41000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.42000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.43000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.44000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.45000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.46000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.47000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.48000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.49000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.50000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.51000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.52000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.53000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.54000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.55000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.56000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.57000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.58000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "sell", "1.00000000 OLIVE", "0.59000000 TLOS",1]' -p alice

echo "-- alice buys OLIVE --"
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.10000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.11000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.12000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.13000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.14000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.15000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.16000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.17000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.18000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.19000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.20000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.21000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.22000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.23000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.24000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.25000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.26000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.27000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.28000000 TLOS",1]' -p alice
cleos push action telosbookdex order '["alice", "buy", "1.00000000 OLIVE", "0.29000000 TLOS",1]' -p alice

echo "-- alice sells CNT --"
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.40000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.41000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.42000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.43000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.44000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.45000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.46000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.47000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.48000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.49000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.50000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.51000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.52000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.53000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.54000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.55000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.56000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.57000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.58000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "10.00000000 CNT", "0.59000000 TLOS",0]' -p alice

echo "-- alice buys CNT --"
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.10000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.11000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.12000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.13000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.14000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.15000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.16000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.17000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.18000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.19000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.20000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.21000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.22000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.23000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.24000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.25000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.26000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.27000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.28000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "10.00000000 CNT", "0.29000000 TLOS",0]' -p alice

echo "-- alice sells BOX --"
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.40000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.41000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.42000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.43000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.44000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.45000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.46000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.47000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.48000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.49000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.50000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.51000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.52000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.53000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.54000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.55000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.56000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.57000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.58000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 BOX", "0.59000000 TLOS",0]' -p alice

echo "-- alice buys BOX --"
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.10000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.11000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.12000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.13000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.14000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.15000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.16000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.17000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.18000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.19000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.20000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.21000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.22000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.23000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.24000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.25000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.26000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.27000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.28000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 BOX", "0.29000000 TLOS",0]' -p alice


echo "-- alice sells QBE --"
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.40000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.41000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.42000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.43000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.44000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.45000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.46000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.47000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.48000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.49000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.50000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.51000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.52000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.53000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.54000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.55000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.56000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.57000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.58000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 QBE", "0.59000000 TLOS",0]' -p alice

echo "-- alice buys QBE --"
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.10000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.11000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.12000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.13000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.14000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.15000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.16000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.17000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.18000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.19000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.20000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.21000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.22000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.23000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.24000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.25000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.26000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.27000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.28000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 QBE", "0.29000000 TLOS",0]' -p alice





echo "-- alice sells ROBO --"
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.40000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.41000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.42000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.43000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.44000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.45000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.46000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.47000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.48000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.49000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.50000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.51000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.52000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.53000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.54000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.55000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.56000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.57000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.58000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 ROBO", "0.59000000 TLOS",0]' -p alice

echo "-- alice buys ROBO --"
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.10000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.11000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.12000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.13000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.14000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.15000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.16000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.17000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.18000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.19000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.20000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.21000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.22000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.23000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.24000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.25000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.26000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.27000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.28000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 ROBO", "0.29000000 TLOS",0]' -p alice





echo "-- alice sells TEACH --"
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.40000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.41000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.42000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.43000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.44000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.45000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.46000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.47000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.48000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.49000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.50000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.51000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.52000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.53000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.54000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.55000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.56000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.57000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.58000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 TEACH", "0.59000000 TLOS",0]' -p alice

echo "-- alice buys TEACH --"
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.10000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.11000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.12000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.13000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.14000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.15000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.16000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.17000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.18000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.19000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.20000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.21000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.22000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.23000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.24000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.25000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.26000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.27000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.28000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 TEACH", "0.29000000 TLOS",0]' -p alice



echo "-- alice sells FAKE --"
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.40000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.41000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.42000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.43000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.44000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.45000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.46000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.47000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.48000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.49000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.50000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.51000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.52000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.53000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.54000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.55000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.56000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.57000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.58000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "sell", "5.00000000 FAKE", "0.59000000 TLOS",0]' -p alice

echo "-- alice buys FAKE --"
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.10000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.11000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.12000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.13000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.14000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.15000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.16000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.17000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.18000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.19000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.20000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.21000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.22000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.23000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.24000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.25000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.26000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.27000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.28000000 TLOS",0]' -p alice
cleos push action telosbookdex order '["alice", "buy", "5.00000000 FAKE", "0.29000000 TLOS",0]' -p alice


echo "-- 7_orders finished --"