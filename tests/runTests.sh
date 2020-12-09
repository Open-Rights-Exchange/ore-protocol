#!/bin/bash

cleos=cleos

echo 'Test Emergency'
./emergency.sh $cleos

echo 'Test Trigger'
./trigger.sh $cleos

echo 'Test Normal Phase'
./normalphase.sh $cleos