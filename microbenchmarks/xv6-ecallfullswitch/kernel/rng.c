//
// Driver for LupIO-RNG
//

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "spinlock.h"

// LupIO RNG is a memory mapped device. This macro returns the address of one
// of the registers.
#define Reg(reg) ((volatile uint32 *)(LUPIO_RNG + reg))

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

// the offsets for the registers
#define RAND 0x0
#define SEED 0x4
#define CTRL 0x8
#define STAT 0xc

// mask for the registers
#define STAT_STATUS 0x1

// values to check status
#define STATUS_BUSY 1

// flags to write
#define ENABLE_INTR 1

static struct spinlock rng_lock;

void
rng_init(void)
{
  initlock(&rng_lock, "rng");
  WriteReg(CTRL, ENABLE_INTR);
}

int
get_random(void)
{
  int rand;
  acquire(&rng_lock);
  while ((ReadReg(STAT) & STAT_STATUS) == STATUS_BUSY) {
    sleep((void *)Reg(STAT), &rng_lock);
  }
  __sync_synchronize();
  rand = ReadReg(RAND);
  release(&rng_lock);
  return rand;
}

void
set_seed(int seed)
{
  acquire(&rng_lock);
  WriteReg(SEED, seed);
  release(&rng_lock);
}

void
rng_intr(void)
{
  acquire(&rng_lock);
  wakeup((void *)Reg(STAT));
  release(&rng_lock);
}
