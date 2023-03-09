#ifndef MEMLAYOUT_H
#define MEMLAYOUT_H

// Physical memory layout

// qemu -machine virt is set up like this,
// based on qemu's hw/riscv/virt.c:
//
// 00001000 -- boot ROM, provided by qemu
// 20000000 -- lupio blk
// 20002000 -- lupio pic
// 20003000 -- lupio rng
// 20004000 -- lupio rtc
// 20005000 -- lupio sys
// 20006000 -- lupio tmr
// 20007000 -- lupio tty
// 80000000 -- boot ROM jumps here in machine mode
//             -kernel loads the kernel here
// unused RAM after 80000000.

// the kernel uses physical memory thus:
// 80000000 -- entry.S, then kernel text and data
// end -- start of kernel page allocation area
// PHYSTOP -- end RAM used by the kernel

// lupio system controller
#define LUPIO_SYS 0x20003000L

// lupio random number generator
#define LUPIO_RNG 0x20005000L
#define LUPIO_RNG_IRQ 3

// lupio real time clock
#define LUPIO_RTC 0x20004000L

// lupio timer registers
#define LUPIO_TMR 0x20006000L

// lupio tty registers
#define LUPIO_TTY 0x20007000L
#define LUPIO_TTY_IRQ 1

// lupio block registers
#define LUPIO_BLK 0x20000000L
#define LUPIO_BLK_IRQ 2

// lupio pic registers
#define LUPIO_PIC 0x20002000L

// the kernel expects there to be RAM
// for use by the kernel and user pages
// from physical address 0x80000000 to PHYSTOP.
#define KERNBASE 0x80000000L
#define PHYSTOP (KERNBASE + 32*1024*1024)

// map the trampoline page to the highest address,
// in both user and kernel space.
#define TRAMPOLINE (MAXVA - PGSIZE)

// map kernel stacks beneath the trampoline,
// each surrounded by invalid guard pages.
#define KSTACK(p) (TRAMPOLINE - ((p)+1)* 2*PGSIZE)

// User memory layout.
// Address zero first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
//   ...
//   TRAPFRAME (p->trapframe, used by the trampoline)
//   TRAMPOLINE (the same page as in the kernel)
#define TRAPFRAME (TRAMPOLINE - PGSIZE)
#endif
