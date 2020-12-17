#!/bin/bash

cleos="$1"

$cleos push action eosio upgraderam '[]' -p eosio

$cleos get table eosio eosio global

$cleos push action system.ore createoreacc '[ "eosio", "test11111152", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn", 1, ""]' -p eosio