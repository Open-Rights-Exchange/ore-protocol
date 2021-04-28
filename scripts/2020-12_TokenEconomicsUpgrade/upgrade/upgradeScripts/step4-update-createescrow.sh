cleos="$1"
createescrow_path="$2"

$cleos set contract -x 1000 createescrow $createescrow_path/ createescrow.wasm createescrow.abi -p createescrow -p system.ore