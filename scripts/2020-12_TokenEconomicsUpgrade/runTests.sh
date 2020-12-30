#!/bin/bash

cleos="$1"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

echo 'Test Emergency'
$DIR/tests/v2/emergency.sh "$cleos"

echo 'Test Trigger'
$DIR/tests/v2/trigger.sh "$cleos"

echo 'Test Normal Phase'
$DIR/tests/v2/normalphase.sh "$cleos"