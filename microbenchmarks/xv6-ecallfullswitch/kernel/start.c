#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

void main();

// entry.S needs one stack per CPU.
__attribute__ ((aligned (16))) char stack0[4096 * NCPU];

// this flag needs to be initialized to a non-zero value to prevent it from
// being stored in the bss
int bss_nflag = 1;

// entry.S jumps here in machine mode on stack0.
void
start(int cpuid)
{
  // disable paging for now.
  w_satp(0);

  // delegate all interrupts and exceptions to supervisor mode.
  w_medeleg(0xffff);
  w_mideleg(0xffff);
  w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

  // configure Physical Memory Protection to give supervisor mode
  // access to all of physical memory.
  volatile void *mtvec = &&pmp_mtvec;
  volatile int cond = 0;

  w_mtvec((uint64)mtvec);

  // prevent gcc from wrongly optimizing label
  if (cond)
    goto *mtvec;

  w_pmpaddr0(0x3fffffffffffffull);
  w_pmpcfg0(0xf);

  asm volatile(".align 2");
pmp_mtvec:
  w_mtvec(0);

  // keep each CPU's hartid in its tp register, for cpuid().
  w_tp(cpuid);

  // set M Previous Privilege mode to Supervisor, for mret.
  unsigned long x = r_mstatus();
  x &= ~MSTATUS_MPP_MASK;
  x |= MSTATUS_MPP_S;
  w_mstatus(x);

  // set M Exception Program Counter to main, for mret.
  // requires gcc -mcmodel=medany
  w_mepc((uint64)main);

  // switch to supervisor mode and jump to main().
  asm volatile("mret");
}
