#include <stdint.h>
#include <stdio.h>
#include "user/syscalls.h"
int db_get_fds[2];
int db_store_fds[2];

void db_part()
{
  uint64_t num = 16;
  write(4, &num, 8);
  read(5, &num, 8);
  printf("%d\n", num);
}
void crypto_part()
{
  uint64_t num;
  read(3, &num, 8);
  printf("%d\n", num);
  num = num + 1;
  write(6, &num, 8);
}
int main (void)
{
  int db_get_fds[2];
  int db_store_fds[2];

  if(pipe(db_get_fds) < 0)
  {
    printf("pipe() failed!");
    return 0;
  }

  if(pipe(db_store_fds) < 0)
  {
    printf("pipe() failed!");
    return 0;
  }

  int pid = fork();

  if (pid < 0)
  {
    printf("fork() failed!");
  }
  if (pid == 0)
  {
    db_part();
    return 0;
  }

  crypto_part();
  wait(0);
  return 0;
}
