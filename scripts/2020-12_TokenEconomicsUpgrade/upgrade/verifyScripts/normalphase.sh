#!/bin/bash

cleos="$1"

sleep 1s
$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global
sleep 1s
$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global
sleep 1s
$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global
sleep 1s
$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global
sleep 1s
$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global
sleep 1s
$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global
sleep 1s
$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global