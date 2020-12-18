#!/bin/bash
cleos="$1"
production_eosio_contracts=${2:-~}/contracts-v1/eosio.contracts
production_ore_contracts=${2:-~}/contracts-v1/ore-protocol
createescrow_path=${2:-~}/contracts-v1/CreateEscrow

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

echo 'Initialize wallets'
$DIR/chain-setup/step0.init_wallet.sh "$cleos" "$2"
echo 'Create accounts'
$DIR/chain-setup/step1.create_accounts.sh "$cleos" "$production_eosio_contracts"
echo 'Vote producer'
$DIR/chain-setup/step2.vote.sh "$cleos"
echo 'Deploy system.ore'
$DIR/chain-setup/step3.oresystem.sh "$cleos" "$production_ore_contracts"
echo 'Deploy createescrow'
$DIR/chain-setup/step4.createescrow.sh "$cleos" "$createescrow_path"

