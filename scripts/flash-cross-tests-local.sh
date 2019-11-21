#!/bin/bash
arm-none-eabi-gdb build-cross-tests/bin/latest.elf -ex "target remote localhost:3333" -ex "load" -ex "monitor reset"  -ex "set confirm off" -ex "quit"


