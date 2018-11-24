
# ORE protocol

The ORE Protocol is a blockchain-hosted API that provides a developer-friendly way to define any digital asset (or any physical asset with an online digital representation) and to trade and control access to these protected resources.

### Rights Token (aka Instrument)
A non-fungible 'Rights Token' can enable access to one or more protected resources. It specifies which resources are available, how much each costs to access, and how to access them. These tokens can be stored in a blockchain 'wallet' and presented to the ORE Protocol to retrieve an endpoint and access token. Payment to access a resource (when required) is made as a real-time micropayment using the stable 'CPU' token.

Protected Resources are endpoints on a network (like an API or IPFS resource) that perform some task (e.g. open a door) or deliver some content (e.g. stream a movie). They can also perform tasks related to issuing, transferring, or trading rights. For example, an endpoint could allow a user to upgrade their right to stream an SD version of a movie to the HD version. It would offer an exchange between one rights token which authorizes streaming of the SD version, along with a payment in CPU, for the HD version token.

We refer to these ‘Rights Tokens’ as Instruments.

### Rights Registry
An Instrument (aka Rights Token) is composed of one or more named rights. Rights names are globally unique and are recorded in the registry which maps a name to a URI or address for the right to be fulfilled - typically an API endpoint. Rights are controlled by their creator account and an Instrument token containing a right can only be issued by a rights owner or authorized issuer account(s) - which are white-listed in this contract.

### EOS Smart Contracts

The Instrument and Rights Registry smart contracts form the core of the ORE protocol. They are contracts deployed on the ORE Network running the EOSIO sofware. 

### ORE Network - an EOS Sister Chain

The ORE Network is a pubic blockchain running (unmodified) EOSIO sofware. ORE has its network of Block Producers - which often overlaps with EOSIO Block Producers. This approach of running a seperate network from the main EOS network that serves a specific use case is commonly reffered to as running an EOS 'Sister Chain'.

Since the ORE chain is only used for storing Instruments (rights tokens) and user accounts (identity). The cost (in tokens) to use the network is much lower than using the main EOS network where users compete for workloads that require more intensive compute and storage (RAM). The cost of using the ORE network is lower and more stable and predictable.

### Interoperability with the EOS Main Net

Accounts and tokens created on the ORE network can interoperate with accounts, tokens, and transactions on the EOS Main Net. EOSIO software was designed to support many parallel 'Sister Chains' that can perform specialized use cases or parition work. EOSIO will release a native feature in the near future called Inter-Blockchain Communication (IBC) that will allow any EOS chain (like the ORE network) to work with transactions on other chains. Until IBC is released, the ORE network interoperates with the EOS Main Net via swap contracts. 

# Workstation Setup

To run a local EOS node, install and start a Docker image using [these instructions](https://developers.eos.io/eosio-home/docs/getting-the-software)

We use these Visual Studio Code with these Extensions:
- C/C++ – IntelliSense, debugging, and code browsing for VS Code
- CMake – CMake language support for Visual Studio Code
- CMake Tools – Extended CMake support in Visual Studio Code
- WebAssembly – Syntax highlight for WebAssembly textual representation

# Code Styleguide

Use the standard defined [here](https://developers.eos.io/eosio-home/docs/coding-standards)
