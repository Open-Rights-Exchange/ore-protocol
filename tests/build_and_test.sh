#!/bin/bash

cd ~/ore-protocol/contracts

cd ore.instrument && mkdir build

eosiocpp -g build/ore.instrument.wast ore.instrument.cpp
eosiocpp -o build/ore.instrument.abi ore.instrument.cpp

cd ../ore.rights_registry && mkdir build
eosiocpp -g build/ore.rights_registry.wast ore.rights_registry.cpp
eosiocpp -o build/ore.rights_registry.abi ore.rights_registry.cpp

cd ../ore.usage_log && mkdir build
eosiocpp -g build/ore.usage_log.wast ore.usage_log.cpp
eosiocpp -o build/ore.usage_log.abi ore.usage_log.cpp

cd ../ore.standard_token && mkdir build
eosiocpp -g build/ore.standard_token.wast ore.standard_token.cpp
eosiocpp -o build/ore.standard_token.abi ore.standard_token.cpp