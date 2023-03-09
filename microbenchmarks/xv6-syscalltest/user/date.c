#include "kernel/types.h"
#include "kernel/time.h"
#include "kernel/stat.h"
#include "user/user.h"

static const char *DAYS[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fir", "Sat"};
static const char *MONTHS[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
                               "Aug", "Sep", "Oct", "Nov", "Dec"};

int
main(int argc, char *argv[])
{
  struct tm time;
  const char *day, *month, *period;
  int sec, min, hour, date, year;

  rdtime(&time);
  day = DAYS[time.tm_wday];
  month = MONTHS[time.tm_mon];
  date = time.tm_mday;
  hour = time.tm_hour % 12 == 0 ? 12 : time.tm_hour % 12;
  min = time.tm_min;
  sec = time.tm_sec;
  period = time.tm_hour < 12 ? "AM" : "PM";
  year = time.tm_year + 1900;
  printf("%s %s %d %d:%d:%d %s GMT %d\n", day, month, date, hour, min, sec, period, year);
  exit(0);
}
