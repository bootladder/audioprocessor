#!/bin/bash
arm-none-eabi-gdb build-prod/bin/latest.elf -ex "target remote localhost:3333" -ex "dump binary value /tmp/dump.bin log_buffer"  -ex "set confirm off" -ex "quit"
cat /tmp/dump.bin

