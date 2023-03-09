#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    fprintf(2, "Shutting down...\n");
    if (reboot(1) != 0) {
      fprintf(2, "Error while shutting down\n");
    }
    exit(1);
  } else if (argc == 2 && !strcmp(argv[1], "-r")) {
    fprintf(2, "Restarting...\n");
    if (reboot(0) != 0) {
      fprintf(2, "Error while restarting\n");
    }
    exit(1);
  } else {
    printf("Usage: %s [-r]\n", argv[0]);
    exit(1);
  }
}
