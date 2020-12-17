#!/bin/bash

cleos="docker exec -it ore-main cleos --url http://127.0.0.1:8888 --wallet-url http://ore-wallet:8901"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

docker-compose -f "$DIR/Dockerfiles/docker-compose-ore.yml" -p ore up -d

$DIR/setup-v1.sh "$cleos" /root

$DIR/upgrade-v2.sh "$cleos" /root