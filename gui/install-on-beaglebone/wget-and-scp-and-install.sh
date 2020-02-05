#!/bin/bash

DESTINATION_ON_TARGET=/mnt/packages/graphviz

file=$(wget $1 2>&1 | grep Saving | cut -d ' ' -f 3 | sed -e 's/[^A-Za-z0-9.\+_-]//g')
scp $file beaglebone-default:$DESTINATION_ON_TARGET
ssh -t beaglebone-default sudo dpkg -i $DESTINATION_ON_TARGET/$file
rm $file

