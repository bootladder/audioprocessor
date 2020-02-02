#!/bin/bash

REMOTETARGET=beaglebone-default
FLASHCOMMAND=/mnt/flash-latest.sh

scp build-prod/bin/latest.elf $REMOTETARGET:/tmp/latest.elf
ssh $REMOTETARGET $FLASHCOMMAND
