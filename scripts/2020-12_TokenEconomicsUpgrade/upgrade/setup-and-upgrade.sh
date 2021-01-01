#!/bin/bash

cleos="$1"
setup_dir="$2"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [[ "$setup_dir" == "" ]]; then
  setup_dir="/root"
fi

$DIR/setup-v1.sh "$cleos" "$setup_dir"
$DIR/upgrade-v2.sh "$cleos" "$setup_dir"
