#!/bin/bash

cleos=$1

$cleos wallet unlock -n development --password $(cat ~/walletpw.txt)

$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global
sleep 2s
$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global
sleep 2s
$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global
sleep 2s
$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global
sleep 2s
$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global
sleep 2s
$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global
sleep 2s
$cleos push action eosio upgraderam '[]' -p eosio
$cleos get table eosio eosio global