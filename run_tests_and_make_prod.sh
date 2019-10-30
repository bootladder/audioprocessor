#!/bin/bash
set -e
PROJECT_DIR=/audio-processor-project
cd $PROJECT_DIR/build-test
cmake .. -DBUILD_TEST=ON
make
bin/unit-tests.elf
cd ..
cd build-prod
cmake .. -DBUILD_TEST=OFF
make
