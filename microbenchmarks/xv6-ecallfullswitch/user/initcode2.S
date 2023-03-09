# Initial process that execs /init.
# This code runs in user space.

#include "syscall.h"

# exec(init, argv)
.globl start
start:
        .4byte 0x8000007b
        ecall
        .4byte 0x8200007b
        jal start
