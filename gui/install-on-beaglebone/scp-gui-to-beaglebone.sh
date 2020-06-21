#!/bin/bash

# Run this script from the project root

ssh beaglebone-default "rm -rf /mnt/deploy && mkdir /mnt/deploy && touch /mnt/deploy/DO_NOT_EDIT_WIPED_AUTOMATICALLY"

scp gui/midi-only.py                             beaglebone-default:/mnt/deploy
scp gui/run-from-beaglebone/flash-latest.sh      beaglebone-default:/mnt/deploy
scp gui/run-from-beaglebone/openocd-command.sh   beaglebone-default:/mnt/deploy
scp gui/run-from-beaglebone/start-midi-only.sh   beaglebone-default:/mnt/deploy