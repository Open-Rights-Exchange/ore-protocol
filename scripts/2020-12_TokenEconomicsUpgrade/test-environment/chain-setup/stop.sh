#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

pkill nodeos

docker-compose -f "$DIR/Dockerfiles/docker-compose-ore.yml" -p ore rm -sf

: ${ORE_DATADIR="$DIR/data"}


obc_dir="${ORE_DATADIR}/blocks"
op2p_dir="${ORE_DATADIR}/p2p"
ostate_dir="${ORE_DATADIR}/state"

# To prevent the risk of glob problems due to non-existant folders,
# we re-create them silently before we touch them.
mkdir -p "$obc_dir" "$op2p_dir" "$ostate_dir" &> /dev/null
rm -rfv "$obc_dir"/*
rm -rfv "$op2p_dir"/*
rm -rfv "$ostate_dir"/*
mkdir -p "$obc_dir" "$op2p_dir" "$ostate_dir" &> /dev/null
rm "${ORE_DATADIR}/nodeos.log"

if [[ "$2" != "/root" ]]; then
  rm ~/eosio-wallet/oretest.wallet
fi