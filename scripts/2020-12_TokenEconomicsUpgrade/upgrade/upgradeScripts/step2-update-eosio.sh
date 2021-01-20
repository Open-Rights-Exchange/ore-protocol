cleos="$1"
eosio_contracts="$2"

sleep 2s
$cleos system newaccount -x 1000 eosio --transfer eosio.upay EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn --stake-net "10000.0000 SYS" --stake-cpu "10000.0000 SYS" --buy-ram-kbytes 8192 -p eosio
sleep 2s
$cleos set contract -x 1000 eosio $eosio_contracts/eosio.system/ -p eosio -p system.ore
sleep 2s
$cleos set contract -x 1000 eosio.token $eosio_contracts/eosio.token/ -p eosio.token -p system.ore
sleep 2s
$cleos push action -x 1000 eosio setram '[17179869184]' -p eosio # 16 Gib
sleep 2s
$cleos set account permission -x 1000 system.ore active '{"threshold": 1,"keys": [{"key": "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn","weight": 1}],"accounts": [{"permission":{"actor":"eosio","permission":"eosio.code"},"weight":1}, {"permission":{"actor":"system.ore","permission":"eosio.code"},"weight":1}]}' owner -p system.ore@owner
sleep 2s
$cleos push action -x 1000 eosio.token open '["eosio.upay", "4,ORE", "eosio"]' -p eosio@active
sleep 2s
$cleos push action -x 1000 eosio.token setstaked '["0.0000 ORE"]' -p eosio.token@active
sleep 2s
$cleos push action -x 1000 eosio setinflation '[0, 10000, 10000]' -p eosio@active