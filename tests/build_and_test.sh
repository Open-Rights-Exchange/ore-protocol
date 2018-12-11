#!/bin/bash

cd ~/ore-protocol/contracts

cd ore.instrument && mkdir build
eosio-cpp ore.instrument.cpp -o build/ore.instrument.wast 
eosio-abigen ore.instrument.cpp --contract=ore.instrument --output=build/ore.instrument.abi

cd ../ore.rights_registry && mkdir build
eosio-cpp ore.rights_registry.cpp -o build/ore.rights_registry.wast 
eosio-abigen ore.rights_registry.cpp --contract=ore.rights_registry --output=build/ore.rights_registry.abi

cd ../ore.usage_log && mkdir build
eosio-cpp ore.usage_log.cpp -o build/ore.usage_log.wast 
eosio-abigen ore.usage_log.cpp --contract=ore.usage_log --output=build/ore.usage_log.abi

cd ../ore.standard_token && mkdir build
eosio-cpp ore.standard_token.cpp -o build/ore.standard_token.wast 
eosio-abigen ore.standard_token.cpp --contract=ore.standard_token --output=build/ore.standard_token.abi
