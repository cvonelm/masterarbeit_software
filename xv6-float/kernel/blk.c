// driver for LupIO's block device
//
// qemu ... -drive file=fs.img,if=none,format=raw

#include <stddef.h>

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"

// LupIO BLK is a memory mapped device. This macro returns the address of one
// of the registers.
#define Reg(reg) ((volatile uint32 *)(LUPIO_BLK + reg))

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

// the offsets for the registers
#define CONF 0x0
#define NBLK 0x4
#define BLKA 0x8
#define MEMA 0xC
#define CTRL 0x10
#define STAT 0x14

// masks for the registers
#define STAT_BUSY 0x1
#define STAT_TYPE (1 << 1)
#define STAT_ERRR (1 << 2)

// values to check the status of the last transfer in the STAT register
#define READ_SUCCESS 0x0
#define READ_ERR 0x4
#define WRITE_SUCCESS 0x2
#define WRITE_ERR 0x6

// values for commands, interrupts are enabled
#define CTRL_READ 0x1
#define CTRL_WRITE 0x3

static int block_size;
static int block_count;
static struct spinlock blk_lock;

// tracks which buf the blk device owns
static volatile struct buf *owned_buf = NULL;

void
blk_init(void)
{
  int block_conf = ReadReg(CONF);
  block_size = 1UL << (block_conf >> 16);
  block_count = 1UL << (block_conf & 0xFFFF);

  // The kernel only reads and writes in chunks of BSIZE, so we will panic if
  // the block size is greater than BSIZE. We could get around this in the
  // future by using a temporary buffer of block size to read and write from.
  if(block_size > BSIZE) {
    panic("device block size is too large");
  }

  initlock(&blk_lock, "blk");
}

// blk_lock must be acquired before calling this function
// The block device will also take ownership of buf b in this function.
void
blk_rw(struct buf *b, int write)
{
  unsigned long addr = (unsigned long)b->data;

  if(addr < KERNBASE || addr >= PHYSTOP || addr + BSIZE > PHYSTOP){
    panic("attempted to access disk with pointer outside of kernel memory");
  }

  acquire(&blk_lock);

  // waits until blk does not own a buf
  // the block device should be ready when no buf is owned
  while(owned_buf != NULL){
    sleep(&owned_buf, &blk_lock);
  }

  // take ownership of buf
  b->disk = 1;
  // take ownership of blk driver
  owned_buf = b;

  __sync_synchronize();

  // configure transfer
  uint64 nblocks = (BSIZE / block_size);
  uint64 dev_blockno = owned_buf->blockno * nblocks;
  WriteReg(NBLK, nblocks);
  WriteReg(BLKA, dev_blockno);
  WriteReg(MEMA, addr);
  __sync_synchronize();
  if (write)
    WriteReg(CTRL, CTRL_WRITE);
  else
    WriteReg(CTRL, CTRL_READ);

  // wait for blk_intr() to say request has finished
  while(owned_buf->disk == 1) {
    sleep((void *)owned_buf, &blk_lock);
  }

  // release ownership of blk driver
  owned_buf = NULL;
  wakeup(&owned_buf);
  release(&blk_lock);
}

void
blk_intr()
{
  acquire(&blk_lock);
  uint32 stat = ReadReg(STAT);
  if(stat == READ_ERR){
    panic("error reading from disk");
  }else if(stat == WRITE_ERR){
    panic("error writing to disk");
  }
  owned_buf->disk = 0;
  wakeup((void *)owned_buf);
  release(&blk_lock);
}
