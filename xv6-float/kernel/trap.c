#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "mode.h"
extern char trampoline[], pcallenter[], pcallret[], kerneltrapret[];

// in kernelvec.S, calls kerneltrap().
void kernelvec();
void kerneltrap();
extern int devintr();

// set up to take exceptions and traps while in the kernel.
void
trapinithart(void)
{
  kernel_trapframe.is_kernel=1;
  kernel_trapframe.kernel_trap = (uint64)kerneltrap;
  w_sscratch((uint64)&kernel_trapframe);
  
  struct mode_entry *kernel_entry = get_entry_for_modeid(1);

  kernel_entry->entry_point = (uint64)TRAMPOLINE + (pcallenter - trampoline);
  kernel_entry->modeid = 1;
  kernel_entry->parent = 1;

  mlbstore(0, (uint64)kernel_entry);
}

//
// handle an interrupt, exception, or system call from user space.
// called from trampoline.S
//
void
usertrap(void)
{
  int which_dev = 0;

  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_sscratch((uint64)&kernel_trapframe);
  struct proc *p = myproc();

  p->trapframe->modeid = r_targetmode();
  p->trapframe->epc = r_sepc();
  p->trapframe->target = r_ccalladdr();
  if(r_scause() == 9){
    // system call

    if(p->killed)
      exit(-1);

    // an interrupt will change sstatus &c registers,
    // so don't enable until done with those registers.
    intr_on();

    syscall();

  } // MLB fault
  else if(r_scause() == 16)
  {
    store_mode(r_stval());
  } // PLB fault
  else if(r_scause() == 17)
  {
    panic("plb fault: unimplemented");
  }
  else if((which_dev = devintr()) != 0){
    // ok
  } else {
    printf("usertrap(): unexpected scause %p stval %p sepc %p pid=%d\n", r_scause(),  r_stval(), r_sepc(),  p->pid);
    p->killed = 1;
  }

  if(p->killed)
    exit(-1);

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2)
    yield();

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

  mode_set_epc(p->trapframe->modeid, p->trapframe->epc);
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

  // set S Exception Program Counter to the saved user pc.
  w_targetmode(p->trapframe->modeid);
  
  uint64 satp = MAKE_SATP(p->pagetable);

  uint64 fn = TRAMPOLINE + (pcallret - trampoline);
  ((void (*)(uint64,uint64))fn)(TRAPFRAME, satp);
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
// This is executed fully with disabled interrupts
void 
kerneltrap()
{
  w_sscratch((uint64)&kernel_trapframe);
  int which_dev = 0;
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();
  uint64 kernel_stack = kernel_trapframe.kernel_sp;
  
  if(intr_get() != 0)
    panic("kerneltrap: interrupts enabled");
  
  if(scause == 16)
  {
    printf("Encountered MLB Fault!");
  }
  else if(scause == 17)
  {
    printf("Encountered PLB Fault!");
  }
  else if((which_dev = devintr()) == 0){
    printf("scause %p\n", scause);
    printf("sepc=%p stval=%p\n", r_sepc(), r_stval());
    panic(")kerneltrap");
  }

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2 && myproc() != 0 && myproc()->state == RUNNING)
    yield();

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_targetmode(1);
  w_sstatus(sstatus);
  ((void (*)(uint64, uint64))kerneltrapret)((uint64)get_entry_for_modeid(1), kernel_stack);
}

// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
int
devintr()
{
  uint64 scause = r_scause();

  if((scause & 0x8000000000000000L) &&
     (scause & 0xff) == 9){
    // this is a supervisor external interrupt, via PIC.

    // irq indicates which device interrupted.
    int irq = pic_get();

    if(irq == LUPIO_TTY_IRQ){
      ttyintr();
    } else if(irq == LUPIO_BLK_IRQ){
      blk_intr();
    } else if(irq == LUPIO_RNG_IRQ) {
      rng_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }


    return 1;
  } else if(scause == 0x8000000000000005L){
    // timer interrupt
    tmrintr();

    return 2;
  } else {
    return 0;
  }
}

