#!/bin/bash
set -e

THISDIR=$(dirname $0)
TESTSDIR=$(dirname $0)/../build-test

cd $TESTSDIR
cmake .. -DBUILD_TEST=ON -DTOOLCHAIN=HOST
make
bin/unit-tests.elf


cd ../scripts
./host_build_prod.sh
