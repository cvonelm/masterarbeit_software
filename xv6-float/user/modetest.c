#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(void)
{
  int tab[16];
  tab[0] = 1;
  permchange(r_modeid(), (uint64)tab, (uint64)tab + 16 * sizeof(tab), 0);
  printf("%d", tab[0]);
  exit(0);
}
