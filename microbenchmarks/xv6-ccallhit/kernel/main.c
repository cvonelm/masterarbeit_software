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
{         struct __attribute__((packed)) plb_entry
         {
             uint64 start;
             uint64 end;
             uint16 modeid;
             uint16 bits;
         };
    w_modeid(1);
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    
    trapinithart();  // install kernel trap vector    
    struct mlb_entry child;
    child.modeid = 2;
    child.parent = 1;
    struct plb_entry perms;

    perms.start = 0x000000000;
    perms.end = 0xffffffffff;
    perms.bits = 0x7;
    perms.modeid = 2;

    plbstore(2, (uint64)&perms);
    mlbstore((uint64)child.modeid, (uint64)&child);
    w_targetmode(2);
    w_ccalladdr((uint64)&ccalltest);
    int i = 0;
    for(;i < 100;i++)
    {
          asm volatile (
               ".4byte  0x8000007b"
          );

      asm volatile(
          "ccall");
    asm volatile (
        ".4byte  0x8200007b"
        );
    }
}

