#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

// LupIO PIC is a memory mapped device. This macro returns the address of one
// of the registers.
#define Reg(reg) ((volatile uint32 *)(LUPIO_PIC + cpuid() * MAX + reg))

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

// the offsets for the registers
#define PRIO 0x0
#define MASK 0x4
#define PEND 0x8
#define ENAB 0xc
// used to distinguish between CPUs
#define MAX  0x10

//
// the LupIO programming interrupt controller (PIC).
//

void
picinithart(void)
{
  int hart = cpuid();

  // Only enable interrupts for CPU 0
  if(hart == 0){
    WriteReg(MASK, (1 << LUPIO_TTY_IRQ) | (1 << LUPIO_BLK_IRQ) | (1 << LUPIO_RNG_IRQ));
    WriteReg(ENAB, 0xFFFFFFFF);
  } else {
    WriteReg(ENAB, 0);
  }
}

// gets the highest priority, unmasked, and active input IRQ
int
pic_get(void)
{
  return ReadReg(PRIO);
}
