## Token Economics V2 Upgrade ##

./upgrade-v2.sh contains the exact steps that has applied for chain upgrade.

./setup-v1.sh and ./chain-setups generates pre-upgrade environment for testing purposes.

To appy upgrade and run tests, from repositories root directory:

```bash
#Run tests using pre-built docker images
./test.sh
```
```bash
#Run tests using local eosio software. (nodeos 1.8.7 & eosio.cdt 1.6.5 required)
./test.sh cleos
```