To test upgrade and verification, from /test-environment directory:

```bash
#Run tests using pre-built docker images
./test.sh
```
```bash
#Run tests using local eosio software. (nodeos 1.8.7 & eosio.cdt 1.6.5 required)
./test.sh cleos
```