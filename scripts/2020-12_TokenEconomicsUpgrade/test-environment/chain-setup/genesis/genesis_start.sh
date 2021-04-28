#!/bin/bash
DATADIR="./blockchain"
nodeos=nodeos


if [ ! -d $DATADIR ]; then
  mkdir -p $DATADIR;
fi

$nodeos \
--genesis-json $DATADIR"/../../genesis.json" \
--signature-provider EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn=KEY:5HyUCyaSAHM9FSomwidgDVcoeBFVs6d3EKF5VZy1fPTFGJXFAZN \
--plugin eosio::producer_api_plugin \
--plugin eosio::chain_api_plugin \
--plugin eosio::http_plugin \
--plugin eosio::history_api_plugin \
--plugin eosio::history_plugin \
--filter-on=* \
--data-dir $DATADIR"/data" \
--blocks-dir $DATADIR"/blocks" \
--config-dir $DATADIR"/config" \
--producer-name eosio \
--http-server-address 127.0.0.1:8888 \
--p2p-listen-endpoint 127.0.0.1:9010 \
--access-control-allow-origin=* \
--contracts-console \
--http-validate-host=false \
--verbose-http-errors \
--enable-stale-production \
--max-transaction-time=1000 \
--p2p-peer-address localhost:9011 \
--p2p-peer-address localhost:9012 \
--p2p-peer-address localhost:9013 \
--p2p-peer-address localhost:9014 \
>> $DATADIR"/nodeos.log" 2>&1 & \
echo $! > $DATADIR"/eosd.pid"
