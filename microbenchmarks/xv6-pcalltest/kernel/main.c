#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "proc.h"

void ccalltest();
void
main()
{
    w_modeid(1);
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    
    trapinithart();  // install kernel trap vector    
    int i = 0;
    for(;i < 100;i++)
    {
                 asm volatile (
                ".4byte  0x8000007b"
           );

      asm volatile(
        "pcall"
        );
           asm volatile (
         ".4byte  0x8200007b"
         );

    }
}

