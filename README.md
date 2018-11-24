
# THIS IS AN ALPHA RELEASE

# ORE protocol

The Open Rights Exchange (ORE) protocol has a notion of `instrument` that is general enough to describe the rights associated with any given economic transaction. Every instrument has a representation on the EOS blockchain that represents its ownership and associated rights. We wish to build a sandbox implementation of this protocol to demonstrate its applicability to facilitating transactions in an API economy. 

# Workstation Setup

EOS Wiki with install instructions and troubleshooting [here](https://github.com/EOSIO/eos/wiki)

To run a local EOS node
```
git clone https://github.com/EOSIO/eos --recursive
cd eos
./eosio_build.sh
/usr/local/bin/mongod -f /usr/local/etc/mongod.conf 
cd build
make test
```

To launch the local EOS node
```
nodeos -e -p eosio --plugin eosio::wallet_api_plugin --plugin eosio::chain_api_plugin --plugin eosio::history_api_plugin --contracts-console
```


We are using Visual Studio Code
Install these VS Code Extensions:
- C/C++ – IntelliSense, debugging, and code browsing for VS Code
- CMake – CMake language support for Visual Studio Code
- CMake Tools – Extended CMake support in Visual Studio Code
- WebAssembly – Syntax highlight for WebAssembly textual representation

# Code Styleguide

Use the standard defined [here](https://github.com/EOSIO/eos/wiki/Coding-Standards)

# Repo structure

This repo was setup using ideas from this post [Setup VSCode for EOS](https://infinitexlabs.com/setup-ide-for-eos-development/)

