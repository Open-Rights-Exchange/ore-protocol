#!/bin/bash

cleos=cleos

production_eosio_contracts=~/ore-protocol/tests/old-contracts/eosio.contracts

production_ore_contracts=~/ore-protocol/tests/old-contracts/ore-protocol

createescrow_path=~/ore-protocol/tests/old-contracts/CreateEscrow

echo 'Create accounts'
./chain-setup/2.create_accounts.sh $cleos $production_eosio_contracts
echo 'Vote producer'
./chain-setup/3.vote.sh $cleos
echo 'Deploy system.ore'
./chain-setup/4.oresystem.sh $cleos $production_ore_contracts
echo 'Deploy createescrow'
./chain-setup/5.createescrow.sh $cleos $createescrow_path
