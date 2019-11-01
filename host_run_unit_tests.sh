#!/bin/bash
set -e

echo $(dirname $0)/build-test
cd $(dirname $0)/build-test
cmake .. -DBUILD_TEST=ON
make
bin/unit-tests.elf
