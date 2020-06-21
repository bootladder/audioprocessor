#!/bin/bash
#gdb-multiarch /tmp/latest.elf -ex "target remote localhost:3333" -ex "load" -ex "monitor reset" -ex "shell sleep 3" -ex "monitor halt" -ex "set confirm off" -ex "quit"
gdb-multiarch /tmp/latest.elf -ex "target remote localhost:3333" -ex "load" -ex "monitor reset" -ex "set confirm off" -ex "quit"