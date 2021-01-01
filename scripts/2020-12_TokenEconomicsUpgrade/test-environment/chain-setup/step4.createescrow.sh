#!/bin/bash
CHAIN_SYMBOL=${3:-ORE}
SYMBOL_PRECISION=${4:-4}

cleos="$1"
createescrow_path=$2

$cleos system newaccount eosio --transfer createescrow EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn --stake-net "10000.0000 SYS" --stake-cpu "10000.0000 SYS" --buy-ram-kbytes 8192

# sleep 3s
$cleos set contract createescrow $createescrow_path/ createescrow.wasm createescrow.abi

$cleos set account permission createescrow active \
'{"threshold": 1,"keys": [{"key": "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn","weight": 1}],"accounts": [{"permission":{"actor":"createescrow","permission":"eosio.code"},"weight":1}, {"permission":{"actor":"system.ore","permission":"eosio.code"},"weight":1}]}' owner

sleep 3s
$cleos push action createescrow init '["'$SYMBOL_PRECISION','$CHAIN_SYMBOL'","system.ore","createoreacc", 0]' -p createescrow

AIRDROP_JSON='{"contract":"", "balance":"0 ORE", "amount":"0 ORE"}'
REX_JSON='{"net_loan_payment":"0.0000 ORE","net_loan_fund":"0.0000 ORE","cpu_loan_payment":"0.0000 ORE","cpu_loan_fund":"0.0000 ORE"}'
PARAMS_JSON='{"owner":"system.ore", "dapp":"test.com", "ram_bytes":"0", "net":"0.0001 ORE", "cpu":"0.0001 ORE", "airdrop":'$AIRDROP_JSON', "pricekey":1, "use_rex": false, "rex":'$REX_JSON'}'
sleep 3s
$cleos push action createescrow define "$PARAMS_JSON" -p system.ore
sleep 3s
$cleos transfer system.ore createescrow "100.0000 ORE" "test.com,50,10" -p system.ore

sleep 3s
$cleos push action createescrow create '["system.ore","producer4231","EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn","EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn","test.com","producer1"]' -p system.ore
