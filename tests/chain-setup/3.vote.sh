#!/bin/bash
cleos=$1

$cleos system voteproducer prods producer1 producer1
$cleos system voteproducer prods producer2 producer1
$cleos system voteproducer prods producer3 producer1