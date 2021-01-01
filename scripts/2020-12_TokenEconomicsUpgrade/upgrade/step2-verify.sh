#!/bin/bash

cleos="$1"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

echo 'Test Emergency'
$DIR/verifyScripts/emergency.sh "$cleos"

echo 'Test Trigger'
$DIR/verifyScripts/trigger.sh "$cleos"

echo 'Test Normal Phase'
$DIR/verifyScripts/normalphase.sh "$cleos"