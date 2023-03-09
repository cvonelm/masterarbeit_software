#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

struct Parameters {
  char *filename;
  int has_length_limit;
  int length;
};

int
parse_args(int argc, char *argv[], struct Parameters *params)
{
  if (argc == 2) {
    params->filename = argv[1];
    params->has_length_limit = 0;
  } else if (argc == 4 && !strcmp(argv[1], "-n")) {
    params->has_length_limit = 1;
    params->length = atoi(argv[2]);
    if (params->length == 0) {
      return -1;
    }
    params->filename = argv[3];
  } else {
    return -1;
  }
  return 0;
}

// This is currently needed because printf does not support width options
// If printf adds width options, this can be removed
// Note: the actual width of n must be less than or equal to the provided width
void
format_hex(int n, int width, char *buf)
{
  static const char digits[] = "0123456789abcdef";
  int i = width - 1;
  buf[width] = '\0';
  do {
    buf[i--] = digits[n % 16];
  } while ((n /= 16) != 0);
  while (i >= 0) {
    buf[i--] = '0';
  }
}

void
print_buf(char *buf, int n, int *bytes_written) {
  char hex_buf[9];

  for (int i = 0; i < n; i++) {
    if (*bytes_written % 16 == 0) {
      if (*bytes_written != 0) {
        printf("\n");
      }
      format_hex(*bytes_written, 8, hex_buf);
      printf("%s", hex_buf);
    } else if (*bytes_written % 8 == 0) {
      printf(" ");
    }
    format_hex(buf[i], 2, hex_buf);
    printf(" %s", hex_buf);
    *bytes_written += 1;
  }
}

void
hexdump(int fd, struct Parameters params)
{
  int bytes_written = 0;
  int n;
  int length_to_read;
  char buf[512];

  while (1) {
    if (params.has_length_limit && params.length - bytes_written < sizeof(buf)) {
      length_to_read = params.length - bytes_written;
    } else {
      length_to_read = sizeof(buf);
    }
    if ((n = read(fd, buf, length_to_read)) == 0) {
      break;
    }
    print_buf(buf, n, &bytes_written);
  }
  printf("\n");
}

int
main(int argc, char *argv[])
{
  struct Parameters params;
  int fd;

  if (parse_args(argc, argv, &params) < 0) {
    printf("Usage: %s [-n length] file\n", argv[0]);
    exit(1);
  }

  if ((fd = open(params.filename, O_RDONLY)) < 0) {
    fprintf(2, "hexdump: cannot open %s\n", params.filename);
    exit(1);
  }
  hexdump(fd, params);
  close(fd);

  exit(0);
}
