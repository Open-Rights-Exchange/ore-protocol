#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cleos="${1:-"docker exec -it ore-main cleos --url http://127.0.0.1:8888 --wallet-url http://ore-wallet:8901"}"
setup_dir="$DIR/scripts/2020-12_TokenEconomicsUpgrade/chain-setup"
if [[ "$cleos" == "cleos" ]]; then
  nodeos --data-dir "$setup_dir"/data \
         --config-dir "$setup_dir"/data \
         --config config.ini \
         --logconf "$setup_dir"/data/logging.json \
         --genesis-json "$setup_dir"/data/genesis.json \
         >> "$setup_dir"/data/nodeos.log  2>&1 &
  sleep 1s
  ./scripts/2020-12_TokenEconomicsUpgrade/setup-and-upgrade.sh "$cleos" "$setup_dir"
  ./scripts/2020-12_TokenEconomicsUpgrade/runTests.sh "$cleos" "$setup_dir"
  ./scripts/2020-12_TokenEconomicsUpgrade/stop.sh "$cleos" "$setup_dir"
else
  docker-compose -f "$setup_dir"/../Dockerfiles/docker-compose-ore.yml -p ore up -d
  sleep 1s
  ./scripts/2020-12_TokenEconomicsUpgrade/setup-and-upgrade.sh "$cleos"
  ./scripts/2020-12_TokenEconomicsUpgrade/runTests.sh "$cleos"
  ./scripts/2020-12_TokenEconomicsUpgrade/stop.sh "$cleos"
fi


