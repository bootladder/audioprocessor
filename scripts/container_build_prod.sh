#!/bin/bash
set -e
PROJECT_DIR=/audio-processor-project
cd $PROJECT_DIR/build-prod
cmake .. -DBUILD_TEST=OFF -DTOOLCHAIN=CROSS
make
