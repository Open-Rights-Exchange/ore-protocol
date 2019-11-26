#!/bin/bash

cd ~/ore-protocol/contracts

cd ore.instrument && mkdir build
cp ~/ore-protocol/contracts/build/ore.instrument/ore.instrument.wasm build/
cp ~/ore-protocol/contracts/build/ore.instrument/ore.instrument.abi build/

cd ../ore.rights_registry && mkdir build
cp ~/ore-protocol/contracts/build/ore.rights_registry/ore.rights_registry.wasm build/
cp ~/ore-protocol/contracts/build/ore.rights_registry/ore.rights_registry.abi build/

cd ../ore.usage_log && mkdir build
cp ~/ore-protocol/contracts/build/ore.usage_log/ore.usage_log.wasm build/
cp ~/ore-protocol/contracts/build/ore.usage_log/ore.usage_log.abi build/

cd ../ore.standard_token && mkdir build
cp ~/ore-protocol/contracts/build/ore.standard_token/ore.standard_token.wasm build/
cp ~/ore-protocol/contracts/build/ore.standard_token/ore.standard_token.abi build/