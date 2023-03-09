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
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    
    struct kvm_satp
    {
      uint64 table1;
      uint64 table2;
      uint64 which;
    };

    struct kvm_satp satps;

    satps.table1 = MAKE_SATP(kernel_pagetable);
    satps.table2 = MAKE_SATP2(kernel_pagetable2);
    satps.which = 0;

    w_sscratch((uint64)&satps);
    trapinithart();  // install kernel trap vector    
    int i = 0;
    for(;i < 100;i++)
    {
                 asm volatile (
                ".4byte  0x8000007b"
           );

      asm volatile(

        "ecall"
        );
      asm volatile(

        "ecall"
        );
           asm volatile (
         ".4byte  0x8200007b"
         );

    }
}

