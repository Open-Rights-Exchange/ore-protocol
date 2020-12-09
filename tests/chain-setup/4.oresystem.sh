#!/bin/bash
cleos=$1

ore_contracts=$2

$cleos wallet unlock -n development --password $(cat ~/walletpw.txt)

$cleos transfer eosio ore "10000000.0000 SYS" "test.com,50,10" -p eosio@active
sleep 1s
$cleos system newaccount ore --transfer system.ore EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn --stake-net "10000.0000 SYS" --stake-cpu "10000.0000 SYS" --buy-ram-kbytes 8192 -p ore
$cleos system newaccount ore --transfer lock.ore EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn --stake-net "10000.0000 SYS" --stake-cpu "10000.0000 SYS" --buy-ram-kbytes 8192 -p ore

# sleep 3s
$cleos set contract system.ore $ore_contracts/ore.system/ ore.system.wasm ore.system.abi

$cleos set account permission system.ore active '{"threshold": 1,"keys": [{"key": "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn","weight": 1}],"accounts": [{"permission":{"actor":"system.ore","permission":"eosio.code"},"weight":1}]}' owner -p system.ore@owner
$cleos set account permission lock.ore active '{"threshold": 1,"keys": [{"key": "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn","weight": 1}],"accounts": [{"permission":{"actor":"system.ore","permission":"eosio.code"},"weight":1}]}' owner -p lock.ore@owner

$cleos transfer eosio producer1 "10000000.0000 ORE" "test.com,50,10" -p eosio@active
$cleos transfer eosio producer1 "10000000.0000 SYS" "test.com,50,10" -p eosio@active
$cleos transfer eosio system.ore "10000000.0000 ORE" "test.com,50,10" -p eosio@active
$cleos transfer eosio system.ore "10000000.0000 SYS" "test.com,50,10" -p eosio@active


sleep 3s
$cleos push action system.ore setprice '[ "200.0000 ORE", 10000, "100.0000 ORE", "100.0000 ORE", 10]' -p system.ore

$cleos push action system.ore setprice '[ "4.0000 ORE", 3048, "0.1000 ORE", "0.1000 ORE", 1]' -p system.ore

$cleos push action system.ore setprice '[ "5.0000 ORE", 4075, "1.0000 ORE", "1.0000 ORE", 2]' -p system.ore

$cleos push action system.ore setprice '[ "6.0000 ORE", 5098, "1.1000 ORE", "1.1000 ORE", 3]' -p system.ore

$cleos push action system.ore setprice '[ "50.0000 ORE", 9000, "20.0000 ORE", "80.000 ORE", 4]' -p system.ore

$cleos push action system.ore setprice '[ "1000000.0000 ORE", 30000, "1000.0000 ORE", "1000.0000 ORE", 100]' -p system.ore
$cleos push action system.ore setprice '[ "1000010.0000 ORE", 30100, "1010.0000 ORE", "1010.0000 ORE", 101]' -p system.ore

#$cleos push action system.ore changetier '["test11111111", 2]' -p test11111111
#cleos transfer eosio test11111113 "100.0000 ORE" "test.com,50,10" -p eosio@active

$cleos set account permission producer1 active '{"threshold": 1,"keys": [{"key": "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn","weight": 1}],"accounts": [{"permission":{"actor":"system.ore","permission":"eosio.code"},"weight":1}]}' owner -p producer1@owner

sleep 3s
$cleos push action system.ore createoreacc '[ "producer1", "neworeacct11", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 1, "producer1"]' -p producer1
$cleos push action system.ore createoreacc '[ "producer1", "neworeacct12", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 1, "producer1"]' -p producer1
$cleos push action system.ore createoreacc '[ "producer1", "test11111111", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 10, "producer1"]' -p producer1
$cleos push action system.ore createoreacc '[ "producer1", "test11111112", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 100, "producer1"]' -p producer1

#cleos set account permission test11111113 active '{"threshold": 1,"keys": [{"key": "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn","weight": 1}],"accounts": [{"permission":{"actor":"system.ore","permission":"eosio.code"},"weight":1}]}' owner -p test11111113@owner

# cleos push action system.ore createoreacc '[ "app.oreid", "eleveneleven", "EOS8WXmxBZkHXZJ4BcWyQp9J7jMgeKLQuXvuNE58SuDYbwkQ35sEC", "EOS88eSdGafNxaUg72vrXua6Y2MvwPxH8hmr8bm4Rka8kwjtWx5Pa", 1, "app.oreid"]' -p app.oreid

# Private key: 5K719KnRR6LQUz9UsrT2um8XeJ3Gk65nf6WZk6HTGfysHVaHNFd
# Public key: EOS8WXmxBZkHXZJ4BcWyQp9J7jMgeKLQuXvuNE58SuDYbwkQ35sEC
# Basarcans-MacBook-Pro:ore.system basarcancelebci$ cleos create key --to-console
# Private key: 5JgKdxGZpw8BHRb5aSkLr1GgMqLEDJJmQYqYoTgQXYfJPUnnBas
# Public key: EOS88eSdGafNxaUg72vrXua6Y2MvwPxH8hmr8bm4Rka8kwjtWx5Pa