#!/bin/bash

cleos=$1
eosio_contracts=$2
ore_contracts=$3

$cleos wallet unlock -n local --password PW5KaVyD3RTq5QER4zHLNXoCxJZLCgMWmvpAiXdwwBvKFp5F1g2TL

$cleos set contract eosio $eosio_contracts/build/contracts/eosio.system/

$cleos set contract eosio.token $eosio_contracts/build/contracts/eosio.token/
# sleep 3s
$cleos set contract system.ore $ore_contracts/build/ore.system/ ore.system.wasm ore.system.abi

$cleos push action eosio setram '[17179869184]' -p eosio # 16 Gib

$cleos push action system.ore migrate '[]' -p system.ore