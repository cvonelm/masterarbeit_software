#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern char trampoline[], pcallenter[], pcallret[], kerneltrapret[];

extern struct mlb_entry modes[NMODE];
// in kernelvec.S, calls kerneltrap().
void kernelvec();
void kerneltrap();

// set up to take exceptions and traps while in the kernel.
void
trapinithart(void)
{
  kernel_trapframe.is_kernel=1;
  kernel_trapframe.kernel_trap = (uint64)kerneltrap;
  w_sscratch((uint64)&kernel_trapframe);

  w_stvec((uint64)TRAMPOLINE + (pcallenter - trampoline));
}

//
// handle an interrupt, exception, or system call from user space.
// called from trampoline.S
//
void
usertrap(void)
{
  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_sscratch((uint64)&kernel_trapframe);
  struct proc *p = myproc();

  p->trapframe->epc = r_sepc();
  if(r_scause() == 9){
  
  } else {
    printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
    for(;;){}
    p->killed = 1;
  }

  usertrapret();
}

//
// return to user space
void
usertrapret(void)
{
  struct proc *p = myproc();

  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(), so turn off interrupts until
  // we're back in user space, where usertrap() is correct.
  intr_off();

  modes[p->modeid - 1].epc = p->trapframe->epc;
  mlbstore(p->modeid, (uint64)&modes[p->modeid - 1]);

  // set up trapframe values that uservec will need when
  // the process next re-enters the kernel.
  p->trapframe->kernel_satp = r_satp();         // kernel page table
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();         // hartid for cpuid()

  // set up the registers that trampoline.S's sret will use
  // to get to user space.
  
  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  
  uint64 satp = MAKE_SATP(p->pagetable);

  uint64 fn = TRAMPOLINE + (pcallret - trampoline);
  ((void (*)(uint64,uint64))fn)(TRAPFRAME, satp);
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
// This is executed fully with disabled interrupts

void ccalltest();

  void 
kerneltrap()
{
  w_sscratch((uint64)&kernel_trapframe);
  uint64 kernel_stack = kernel_trapframe.kernel_sp;
  ((void (*)(uint64, uint64))kerneltrapret)((uint64)kernelmlb(), kernel_stack);
}
