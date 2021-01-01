#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cleos="${1:-"docker exec -it ore-main cleos --url http://127.0.0.1:8888 --wallet-url http://ore-wallet:8901"}"
setup_dir="$DIR/chain-setup"
if [[ "$cleos" == "cleos" ]]; then
  nodeos --data-dir "$setup_dir"/data \
         --config-dir "$setup_dir"/data \
         --config config.ini \
         --logconf "$setup_dir"/data/logging.json \
         --genesis-json "$setup_dir"/data/genesis.json \
         >> "$setup_dir"/data/nodeos.log  2>&1 &
  sleep 1s
  ./step0-setup-chain.sh "$cleos" "$setup_dir"
  ../upgrade/step1-upgrade.sh "$cleos" "$setup_dir"
  ../upgrade/step2-verify.sh "$cleos" "$setup_dir"
  ./chain-setup/stop.sh "$cleos" "$setup_dir"
else
  docker-compose -f "$setup_dir"/Dockerfiles/docker-compose-ore.yml -p ore up -d
  sleep 1s
  ./step0-setup-chain.sh "$cleos" /root
  ../upgrade/step1-upgrade.sh "$cleos" /root
  ../upgrade/step2-verify.sh "$cleos" /root
  ./chain-setup/stop.sh "$cleos" /root
fi


# cleos="$1"
# setup_dir="$2"

# DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# if [[ "$setup_dir" == "" ]]; then
#   setup_dir="/root"
# fi

# $DIR/setup-v1.sh "$cleos" "$setup_dir"
# $DIR/upgrade-v2.sh "$cleos" "$setup_dir"
