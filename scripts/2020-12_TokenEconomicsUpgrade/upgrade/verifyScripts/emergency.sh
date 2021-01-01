#!/bin/bash

cleos="$1"

$cleos push action system.ore createoreacc '[ "producer1", "test11111122", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 1, "producer1"]' -p producer1

$cleos get table eosio.token ORE stakestats
$cleos get table eosio.token producer1 reserves
$cleos get table eosio.token test11111122 stakeinfo

$cleos push action system.ore settier '[ 100, 1000000000, 20320875724 , "1.0000 ORE", "1.0000 ORE"]' -p system.ore

$cleos push action system.ore settier '[ 20, 30000, 2320875724 , "1.0000 ORE", "1.0000 ORE"]' -p system.ore

$cleos push action system.ore settier '[ 10, 30000, 220875724 , "1.0000 ORE", "1.0000 ORE"]' -p system.ore

$cleos push action system.ore createoreacc '[ "eosio", "test11111124", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 100, ""]' -p eosio
$cleos push action system.ore createoreacc '[ "eosio", "test11111125", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 100, ""]' -p eosio
$cleos push action system.ore createoreacc '[ "eosio", "test11111131", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 100, ""]' -p eosio
$cleos push action system.ore createoreacc '[ "eosio", "test11111132", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 100, ""]' -p eosio
$cleos push action system.ore createoreacc '[ "eosio", "test11111133", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 20, ""]' -p eosio
$cleos push action system.ore createoreacc '[ "eosio", "test11111134", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 20, ""]' -p eosio
$cleos push action system.ore createoreacc '[ "eosio", "test11111135", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 10, ""]' -p eosio
$cleos push action system.ore createoreacc '[ "eosio", "test11111141", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 10, ""]' -p eosio

$cleos get table eosio eosio global

$cleos push action system.ore createoreacc '[ "eosio", "test11111151", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 1, ""]' -p eosio
 