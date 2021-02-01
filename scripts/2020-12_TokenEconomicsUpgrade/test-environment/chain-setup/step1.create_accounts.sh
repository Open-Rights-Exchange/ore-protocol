#!/bin/bash

cleos="$1"
eosio_contracts=$2

$cleos create account eosio eosio.bpay EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn
$cleos create account eosio eosio.msig EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn
$cleos create account eosio eosio.names EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn
$cleos create account eosio eosio.ram EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn
$cleos create account eosio eosio.ramfee EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn
$cleos create account eosio eosio.saving EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn
$cleos create account eosio eosio.stake EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn
$cleos create account eosio eosio.token EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn
$cleos create account eosio eosio.vpay EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn
$cleos create account eosio eosio.rex EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn
$cleos create account eosio eosio.wrap EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn

$cleos set contract eosio $eosio_contracts/eosio.bios/

sleep 1s
$cleos set contract eosio.token $eosio_contracts/eosio.token/

sleep 1s
$cleos set contract eosio.msig $eosio_contracts/eosio.msig/

sleep 1s
$cleos set contract eosio.wrap $eosio_contracts/eosio.wrap/

curl -X POST http://127.0.0.1:8889/v1/producer/schedule_protocol_feature_activations -d '{"protocol_features_to_activate": ["0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd"]}'
curl -X POST http://127.0.0.1:8888/v1/producer/schedule_protocol_feature_activations -d '{"protocol_features_to_activate": ["0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd"]}'

sleep 3s
$cleos set contract eosio $eosio_contracts/eosio.system/
sleep 3s
$cleos set contract eosio $eosio_contracts/eosio.system/

sleep 3s
$cleos push action eosio.token create '[ "eosio", "10000000000.0000 SYS" ]' -p eosio.token@active
                                                   
sleep 1s
$cleos push action eosio.token create '[ "eosio", "10000000000.0000 ORE" ]' -p eosio.token@active

sleep 1s
$cleos push action eosio.token issue '[ "eosio", "1000000000.0000 SYS", "memo" ]' -p eosio@active

sleep 1s
$cleos push action eosio.token issue '[ "eosio", "1000000000.0000 ORE", "memo" ]' -p eosio@active

sleep 3s
$cleos push action eosio init '[0, "4,SYS"]' -p eosio@active

# sleep 3s
$cleos push action eosio setpriv '["eosio.msig", 1]' -p eosio@active

$cleos system newaccount eosio --transfer producer1 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-net "200000000.0000 SYS" --stake-cpu "200000000.0000 SYS" --buy-ram-kbytes 8192

$cleos system newaccount eosio --transfer producer2 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-net "150000000.0000 SYS" --stake-cpu "150000000.0000 SYS" --buy-ram-kbytes 8192

$cleos system newaccount eosio --transfer ore EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn --stake-net "100000000.0000 SYS" --stake-cpu "100000000.0000 SYS" --buy-ram-kbytes 8192

$cleos system newaccount eosio --transfer funds.ore EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn --stake-net "10000.0000 SYS" --stake-cpu "10000.0000 SYS" --buy-ram-kbytes 8192

sleep 3

$cleos system regproducer producer1 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV https://accountnum11.com
$cleos system regproducer producer2 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV https://accountnum12.com

