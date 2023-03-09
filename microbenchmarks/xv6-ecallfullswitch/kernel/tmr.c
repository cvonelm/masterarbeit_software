#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"

// LupIO timer is a memory mapped device. This macro returns the address of one
// of the registers.
#define Reg(reg) ((volatile uint32 *)(LUPIO_TMR + cpuid() * MAX + reg))

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

// the offsets for the registers
#define TIME 0x0
#define LOAD 0x4
#define CTRL 0x8
#define STAT 0xc
// used to distinguish between CPUs
#define MAX  0x10

struct spinlock tickslock;
uint ticks;

void
tmrinit(void)
{
  initlock(&tickslock, "time");
}

void
tmrinithart(void)
{
  int interval = 1000000; // cycles, about about 1/10th second in qemu
  WriteReg(LOAD, interval);
  WriteReg(CTRL, 0x3);
}

void
tmrintr(void)
{
  ReadReg(STAT); // lowers irq
  if(cpuid() == 0) {
    acquire(&tickslock);
    ticks++;
    wakeup(&ticks);
    release(&tickslock);
  }
}
