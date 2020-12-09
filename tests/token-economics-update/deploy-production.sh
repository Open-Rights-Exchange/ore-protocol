#!/bin/bash

cleos=$1
eosio_contracts=$2
ore_contracts=$3
createescrow_path=$4


$cleos system newaccount eosio --transfer eosio.upay EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn --stake-net "10000.0000 SYS" --stake-cpu "10000.0000 SYS" --buy-ram-kbytes 8192 -p eosio

$cleos set contract eosio $eosio_contracts/eosio.system/

$cleos set contract eosio.token $eosio_contracts/eosio.token/

# sleep 3s
$cleos set contract system.ore $ore_contracts/build/ore.system/ ore.system.wasm ore.system.abi

# sleep 3s
$cleos set contract createescrow $createescrow_path/ createescrow.wasm createescrow.abi

$cleos push action eosio setram '[17179869184]' -p eosio # 16 Gib

$cleos set account permission system.ore active '{"threshold": 1,"keys": [{"key": "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn","weight": 1}],"accounts": [{"permission":{"actor":"eosio","permission":"eosio.code"},"weight":1}, {"permission":{"actor":"system.ore","permission":"eosio.code"},"weight":1}]}' owner -p system.ore@owner

$cleos push action eosio.token open '["eosio.upay", "4,ORE", "eosio"]' -p eosio@active

$cleos push action eosio.token setstaked '["0.0000 ORE"]' -p eosio.token@active

$cleos push action eosio setinflation '[0, 10000, 10000]' -p eosio@active

$cleos push action eosio claimrewards '["producer1"]' -p producer1@active

$cleos transfer eosio test11111112 "100.0000 ORE" "test.com,50,10" -p eosio@active

$cleos push action system.ore migrate '[]' -p system.ore