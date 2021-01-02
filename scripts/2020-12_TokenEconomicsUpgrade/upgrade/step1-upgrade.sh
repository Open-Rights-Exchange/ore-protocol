#!/bin/bash
#
# To run the upgrade there must be a wallet unlock with following keys:
# eosio@active, eosio.token@active, createescrow@active, system.ore@owner, system.ore@active
#
cleos="$1"
eosio_contracts=${2:-~}/contracts-v2/eosio.contracts
createescrow_path=${2:-~}/contracts-v2/CreateEscrow
if [[ "$2" == "/root" ]]; then
  ore_contracts="$2/contracts"
else
  ore_contracts="../../../contracts"
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

sleep 2s
$DIR/upgradeScripts/step1-unstake-ram.sh $cleos 
read -p "press key" keypress
$DIR/upgradeScripts/step2-update-eosio.sh "$cleos" "$eosio_contracts"
read -p "press key" keypress
$DIR/upgradeScripts/step3-update-ore-system.sh "$cleos" "$ore_contracts"
read -p "press key" keypress
$DIR/upgradeScripts/step4-update-createescrow.sh "$cleos" "$createescrow_path"