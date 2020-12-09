#!/bin/bash

cleos=cleos

eosio_contracts=~/ore-protocol/tests/external/eosio.contracts

ore_contracts=~/ore-protocol/contracts

createescrow_path=~/ore-protocol/tests/external/CreateEscrow


echo 'Update production'
./token-economics-update/deploy-production.sh $cleos $eosio_contracts $ore_contracts $createescrow_path
