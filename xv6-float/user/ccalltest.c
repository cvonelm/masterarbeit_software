#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "user/syscalls.h"
#include "kernel/syscall.h"
#include "user/libmode.h"

void *ccall(void *arg)
{
  access("a");
  printf("ARG is: %d\n", *(int *)arg);
  access("t");
  return (void *)10;
}

int syscallhandler(int a0, int a1, int a2, int a3, int a4, int a5)
{
  uint64_t sys_nr = r_ccalladdr();
  sys_nr &= SYS_MASK;
  if(sys_nr == SYS_access)
  {
    char *path = (char *)a0;
    if(path[0] == 't')
    {
      printf("Evil Program doing evil program things!\n");
      return -1;
    }
  }
  return scall(a0, a1, a2, a3, a4, a5);
}
int main()
{
  int i;
  while(1)
  {
       asm volatile (
 
                   ".4byte  0xb400007b"
              );
       i++;
                     asm volatile (
            ".4byte  0xb600007b"
            );
   }


  struct mode *mode = modeinit();
  mode->syscallhandler = &syscallhandler;
  int child = modecreate(mode);
  int arg = 5;
  int ret = (int)childcall(mode, child, &ccall, &arg);
  printf("Return ist: %d\n", ret);
  modedelete(child);
  modefinish(mode);
  return 0;
}
