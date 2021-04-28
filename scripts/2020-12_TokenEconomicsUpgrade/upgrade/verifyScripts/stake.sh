#!/bin/bash

cleos="$1"

$cleos push action system.ore createoreacc '[ "producer1", "test11111122", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 1, "producer1"]' -p producer1

$cleos get table eosio.token ORE stakestats
$cleos get table eosio.token producer1 reserves
$cleos get table eosio.token test11111122 stakeinfo

$cleos push action system.ore settier '[ 100, 1000000000, 16320875724 , "1.0000 ORE", "1.000 ORE"]' -p system.ore

$cleos push action system.ore createoreacc '[ "eosio", "test11111123", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 100, ""]' -p eosio
    