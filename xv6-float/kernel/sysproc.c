#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "time.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  /*
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  */
  return 0;
}

uint64 sys_mcreate(void)
{
  struct proc *p =  myproc();
  int modeid = p->trapframe->target >> 47;
  return proc_mcreate(modeid);
}

uint64 sys_mdelete(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;

  return proc_mdelete(n);
}

uint64 sys_permchange()
{
  int modeid, bits;
  int start, end;

  if(argint(0, &modeid) < 0)
    return -1;
  if(argint(1, &start) < 0)
    return -1;
  if(argint(2, &end) < 0)
    return -1;
  if(argint(3, &bits) < 0)
    return -1;

  return mode_permchange(modeid, start, end, bits);
}
uint64 sys_mcntl(void)
{
  int bits;
  if(argint(0, &bits) < 0)
    return -1;
  struct proc *p =  myproc();
  int modeid = p->trapframe->target >> 47;
  return mode_mcntl(modeid, bits);

}

uint64 sys_mentry(void)
{
  struct proc *p = myproc();
  int n;
  if(argint(0, &n) < 0)
    return -1;

  int modeid = p->trapframe->target >> 47;
  return mode_set_entry_point(modeid, n);
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// 0 to reboot, 1 to halt the vm
uint64
sys_reboot(void)
{
  uint ticks0;
  int flag;

  argint(0, &flag);
  if(flag == 0){
    reboot(0);
  } else {
    halt(0);
  }

  // waits two ticks before returning to see if the shutdown succeeds
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < 2){
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);

  return -1;
}

// Given a pointer to a tm struct, it places its memory in it.
uint64
sys_rdtime(void)
{
  uint64 addr;
  struct tm time;

  if(argaddr(0, &addr) < 0)
    return -1;  
  rdtime(&time);
  return copyout(myproc()->pagetable, addr, (char *)&time, sizeof(time));
}
