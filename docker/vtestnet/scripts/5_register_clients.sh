#!/bin/bash
echo "-- 5_register_clients --"

cleos push action telosbookdex addclient '["bob",   "vapaee","fee memo for vapaee", "Vapaée Telos DEX", "https://vapaee.io", "brief", "banner", "thumbnail"]' -p bob@active
cleos push action telosbookdex addclient '["alice", "sqrl", "fee memo for sqrl", "SQRL Wallet", "https://sqrlwallet.io", "brief", "banner", "thumbnail"]' -p alice@active

echo "-- 5_register_clients finished --"
