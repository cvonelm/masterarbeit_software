// Supplies random bytes through a device file

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"

int
random_read(int user_dst, uint64 dst, int n)
{
  int rand;
  int length_to_write;
  int target = n;

  while(n > 0) {
    rand = get_random();
    length_to_write = n >= sizeof(rand) ? sizeof(rand) : n;
    if(either_copyout(user_dst, dst, &rand, length_to_write) == -1) {
      break;
    }
    dst += length_to_write;
    n -= length_to_write;
  }
  return target - n;
}

int
random_write(int user_src, uint64 src, int n)
{
  // This file is not writable
  return -1;
}

void
random_init(void)
{
  devsw[RANDOM].read = random_read;
  devsw[RANDOM].write = random_write;
}
