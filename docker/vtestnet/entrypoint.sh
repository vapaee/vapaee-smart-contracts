#!/bin/bash

set -e
# state-history-endpoint = 127.0.0.1:8080

# sed -i "/state-history-endpoint/c\state-history-endpoint = 127.0.0.1:8888" ~/.local/share/eosio/nodeos/config/config.ini

# Step 1.1: Start keosd
keosd --unlock-timeout=999999 &

# Step 1.2: Start nodeos
nodeos -e -p eosio \
--plugin eosio::producer_plugin \
--plugin eosio::producer_api_plugin \
--plugin eosio::chain_api_plugin \
--plugin eosio::http_plugin \
--plugin eosio::history_plugin \
--plugin eosio::history_api_plugin \
--filter-on="*" \
--http-server-address=0.0.0.0:8888 \
--access-control-allow-origin='*' \
--contracts-console \
--http-validate-host=false \
--verbose-http-errors >> /tmp/nodeos.log 2>&1 &


sleep 1

cleos wallet create --to-console

sleep 1

cleos wallet import --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3
cleos wallet import --private-key 5J1astpVJcAJVGX8PGWN9KCcHU5DMszi4gJgCEpWc5DxmpTsKqp


#cd /scripts/
# ./init.sh
sleep 1

echo "----------"
echo "tail -f /tmp/nodeos.log"
echo "wget -q -O - ""$@"" http://localhost:8888/v1/chain/get_info"
echo "cd /scripts/; ./init.sh"
echo "----------"
bash