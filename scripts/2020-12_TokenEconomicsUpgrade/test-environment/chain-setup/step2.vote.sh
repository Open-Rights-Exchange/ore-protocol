#!/bin/bash
cleos="$1"

$cleos system voteproducer prods producer1 producer1
$cleos system voteproducer prods producer2 producer2
$cleos system voteproducer prods ore producer1 producer2