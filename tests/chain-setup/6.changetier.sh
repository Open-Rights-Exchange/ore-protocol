#!/bin/bash
cleos=$1
eosio_contracts=$2
ore_eosio_contracts=$3

$cleos wallet unlock -n development --password $(cat ~/walletpw.txt)

$cleos transfer system.ore test11111111 "100.0000 ORE" "test.com,50,10" -p system.ore

sleep 1s

$cleos push action system.ore chgacctier '["producer1", "test11111111", 1]' -p producer1

sleep 1s
$cleos push action system.ore chgacctier '["test11111111", "test11111111", 1]' -p test11111111

sleep 1s
$cleos push action system.ore chgacctier '["system.ore", "test11111111", 3]' -p system.ore

