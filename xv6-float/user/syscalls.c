#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/fcntl.h"
#include "syscalls.h"

int _lseek(int fd, int offset, int whence)
{
  return lseek(fd, offset, whence);
}

void _exit(int status)
{
  exit(status);
}

void *_sbrk(int incr)
{
  return sbrk(incr);
}

int _write(int fd, void *ptr, int len)
{
  return write(fd, ptr, len);
}

int _close(int fd)
{
  return close(fd);
}

int _read(int fd, void *ptr, int len)
{
  return read(fd, ptr, len);
}
