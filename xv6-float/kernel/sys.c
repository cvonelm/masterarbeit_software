//
// Driver for LupIO-SYS
//

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"
#include "param.h"

// LupIO SYS is a memory mapped device. This macro returns the address of one
// of the registers.
#define Reg(reg) ((volatile uint32 *)(LUPIO_SYS + reg))

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

// the offsets for the registers
#define HALT 0x0
#define REBT 0x4

// the interpretation of n is implementation-dependant
void
halt(int n)
{
  WriteReg(HALT, n);
}

// the interpretation of n is implementation-dependant
void
reboot(int n)
{
  WriteReg(REBT, n);
}
