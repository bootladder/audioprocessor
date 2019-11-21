#!/bin/bash

scp build-prod/bin/latest.elf mcbain-home:/tmp/latest.elf
ssh mcbain-home cheatsheet/flash-tmp-latest.sh
