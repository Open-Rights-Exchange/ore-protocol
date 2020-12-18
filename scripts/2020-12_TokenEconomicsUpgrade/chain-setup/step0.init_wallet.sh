#!/bin/bash
cleos="$1"
setup_dir="$2"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [[ "$setup_dir" == "/root" ]]; then
  setup_dir=""
fi
$cleos wallet create -n oretest -f "$setup_dir"/data/walletpw.txt

$cleos wallet unlock -n oretest --password $(cat ${DIR}/data/walletpw.txt)

$cleos wallet import -n oretest --private-key 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3
$cleos wallet import -n oretest --private-key 5JUzsJi7rARZy2rT5eHhcdUKTyVPvaksnEKtNWzyiBbifJA1dUW
$cleos wallet import -n oretest --private-key 5HrMPq41QRVWbQhWvtcxBKLAS5RkksP71q4MumAt2Pdk5SrPpDG
$cleos wallet import -n oretest --private-key 5HyUCyaSAHM9FSomwidgDVcoeBFVs6d3EKF5VZy1fPTFGJXFAZN
