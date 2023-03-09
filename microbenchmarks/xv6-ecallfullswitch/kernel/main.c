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
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is bootingdsaff\n");
    printf("\n");
    kinit();         // physical page allocator
    printf("physical pages allocated");
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    procinit();      // process table
    trapinithart();  // install kernel trap vector
    tmrinit();       // timer
    tmrinithart();   // start timer
    picinithart();   // ask PIC for device interrupts
    rng_init();      // rng device
    random_init();   // random device file
    userinit();      // first user process
    userinit2();
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
