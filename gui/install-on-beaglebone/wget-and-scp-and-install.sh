#!/bin/bash

file=$(wget $1 2>&1 | grep Saving | cut -d ' ' -f 3 | sed -e 's/[^A-Za-z0-9.\+_-]//g')
scp $file beaglebone-default:/mnt/packages/python-tk
ssh -t beaglebone-default sudo dpkg -i /mnt/packages/python-tk/$file
rm $file

