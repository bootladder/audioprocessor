#!/bin/bash
set -e

# this script is assumed to be running in side of the container

PROJECT_DIR=/audio-processor-project
cd $PROJECT_DIR/build-cross-tests
cmake .. -DTOOLCHAIN=CROSS -DBUILD_TEST=ON
make
