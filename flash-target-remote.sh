#!/bin/bash

scp build/bin/latest.elf mcbain-home:/tmp/latest.elf
ssh mcbain-home cheatsheet/flash-tmp-latest.sh
