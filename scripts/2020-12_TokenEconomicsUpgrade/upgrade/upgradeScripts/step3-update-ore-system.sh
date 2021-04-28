cleos="$1"
ore_contracts="$2"

$cleos set contract -x 1000 system.ore $ore_contracts/build/ore.system/ ore.system.wasm ore.system.abi
sleep 2s
$cleos set account permission -x 1000 system.ore active '{"threshold": 1,"keys": [{"key": "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn","weight": 1}],"accounts": [{"permission":{"actor":"eosio","permission":"eosio.code"},"weight":1}, {"permission":{"actor":"system.ore","permission":"eosio.code"},"weight":1}]}' owner -p system.ore@owner
sleep 2s
$cleos push action -x 1000 system.ore migrate '[]' -p system.ore