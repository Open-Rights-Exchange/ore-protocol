#! /bin/bash

printf "\t=========== Building eosio.contracts ===========\n\n"

CURDIRNAME=${PWD##*/}

if [ $CURDIRNAME != 'contracts' ]
then
    cd contracts
fi

RED='\033[0;31m'
NC='\033[0m'

CORES=`getconf _NPROCESSORS_ONLN`
mkdir -p build
pushd build &> /dev/null
cmake ../
make -j${CORES}
popd &> /dev/null
