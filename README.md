
# ORE protocol

The ORE Protocol is a blockchain identity standard that operates across public blockhains. The protocol allows a developer to add an identity to the ORE Idenitty Registry - which contains a hierarchy of public keys. One owner key owns a collection of public keys for one or more blockchains. The protocol also includes the concept of Active NFTs. Possession of these NFTs allows a user to prove their right to access an off chain API. This provides a developer-friendly way to define any digital asset (or any physical asset with an online digital representation) and to trade and control access to these protected resources.

### Rights Token (aka Active NFT)
A non-fungible 'Rights Token' can enable access to one or more protected resources. It specifies which resources are available, how much each costs to access, and how to access them. These tokens can be stored in a blockchain 'wallet' and presented to the ORE Protocol to retrieve an endpoint and access token. Payment to access a resource (when required) is made as a real-time micropayment using on-chain tokens.

Protected Resources are endpoints on a network (like an API or IPFS resource) that perform some task (e.g. open a door) or deliver some content (e.g. stream a movie). They can also perform tasks related to issuing, transferring, or trading rights. For example, an endpoint could allow a user to upgrade their right to stream an SD version of a movie to the HD version. It would offer an exchange between one rights token which authorizes streaming of the SD version, along with a payment in CPU, for the HD version token.

We refer to these ‘Rights Tokens’ as Active NFTs to differentiate them from standard NFTs that are primarily used on-chain.

### Rights Registry
An Instrument (aka Rights Token) is composed of one or more named rights. Rights names are globally unique and are recorded in the registry which maps a name to a URI or address for the right to be fulfilled - typically an API endpoint. Rights are controlled by their creator account and an Instrument token containing a right can only be issued by a rights owner or authorized issuer account(s) - which are white-listed in this contract.

### Managed by Smart Contracts

The ORE Identity Registry, Active NFTs, and the  Rights Registry smart contracts form the core of the ORE protocol. They are contracts deployed on the ORE Blockchain. 

### Interoperability with Ethereum, Algroand, EOS, and other blockchains. 

Accounts and created on the ORE network can own and control a set off public / private keypairs that allow a user to control wallets on multiple chains. This protocol provides a single sign-on experience for applications on multiple chains, sparing most users the complexity of multiple wallets.This cross-chain interoperability is accompllished using the open source Chainjs standard. We welcome new chains that would like to add their libraries to the system. Please visit https://github.com/Open-Rights-Exchange/chain-js to learn more.

### ORE ID & the ORE Blockchain
ORE ID is a service that creates an even simpler user experience by mapping a user's Web 2.0 login to a owenr and active keys in the Identity registry. This allows business to migrate users to one or more blocckhains at scale, and allows users to participate in blockchain applications with commonly used authenticaiotn services like GooglID, AppleID, Facebook, email, etc. To learn more about ORE ID visit: https://oreid.io

### ORE Network

The ORE Network is a pubic blockchain running (unmodified) EOSIO sofware. ORE has its network of Block Producers - which often overlaps with EOSIO Block Producers. This approach of running a seperate network from the main EOS network that serves a specific use case is commonly reffered to as running an EOS 'Sister Chain'.

Since the ORE chain is only used for storing Instruments (rights tokens) and user accounts (identity). The cost (in tokens) to use the network is much lower than using the main EOS network where users compete for workloads that require more intensive compute and storage (RAM). The cost of using the ORE network is lower and more stable and predictable.

# Workstation Setup

To run a local EOS node, install and start a Docker image using [these instructions](https://developers.eos.io/eosio-home/docs/getting-the-software)

We use these Visual Studio Code with these Extensions:
- C/C++ – IntelliSense, debugging, and code browsing for VS Code
- CMake – CMake language support for Visual Studio Code
- CMake Tools – Extended CMake support in Visual Studio Code
- WebAssembly – Syntax highlight for WebAssembly textual representation

# Code Styleguide

Use the standard defined [here](https://developers.eos.io/eosio-home/docs/coding-standards)
