## Token Economics V2 Upgrade ##

/test-environment contains files to generate (dockerized or local build) test EOSIO chain, deploy contracts to emulate pre-upgrade chain state.

/upgrade contains the actual script that is used to deploy ORE v2 contracts (step1) also verification scripts to monitor chain's system status.

To apply upgrade and run tests, from /test-environment directory:

```bash
#Run tests using pre-built docker images
./test.sh
```
```bash
#Run tests using local eosio software. (nodeos 1.8.7 & eosio.cdt 1.6.5 required)
./test.sh cleos
```