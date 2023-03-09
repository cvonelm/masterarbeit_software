#ifndef TIME_H
#define TIME_H
struct tm {
  int tm_sec;  // second from 0-60, where 60 is leap second
  int tm_min;  // minute from 0-59
  int tm_hour; // hour from 0-23
  int tm_mday; // day of month from 1-31
  int tm_mon;  // month from 0-11
  int tm_year; // year - 1900
  int tm_wday; // day of the week from 0-6, Sunday = 0
  int tm_yday; // day of the year from 0-365, where 365 is used during leap years
};
#endif
