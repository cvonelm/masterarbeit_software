#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){
    w_modeid(1);
    w_sstatus(r_sstatus() | SSTATUS_FS);

    consoleinit();
    printfinit();
    printf("xv6 kernel is booting\r\n");
    printf("\n");

    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    modeinit();
    procinit();      // process table

    trapinithart();  // install kernel trap vector
    tmrinit();       // timer
    tmrinithart();   // start timer
    picinithart();   // ask PIC for device interrupts
    binit();         // buffer cache
    iinit();         // inode table
    fileinit();      // file table
    blk_init();      // emulated hard disk
    rng_init();      // rng device
    random_init();   // random device file
    userinit();      // first user process
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    tmrinithart();    // start timer
    picinithart();    // ask PIC for device interrupts
  }

  scheduler();        
}
