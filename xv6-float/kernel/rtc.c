//
// Driver for LupIO-RTC
//

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "time.h"

// LupIO RTC is a memory mapped device. This macro returns the address of one
// of the registers.
#define Reg(reg) ((volatile uint8 *)(LUPIO_RTC + reg))

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

// the offsets for the registers
#define SECD 0x0
#define MINT 0x1
#define HOUR 0x2
#define DYMO 0x3
#define MNTH 0x4
#define YEAR 0x5
#define CENT 0x6
#define DYWK 0x7
#define DYYR 0x8

void
rdtime(struct tm *time)
{
  time->tm_sec = (int)ReadReg(SECD);
  time->tm_min = (int)ReadReg(MINT);
  time->tm_hour = (int)ReadReg(HOUR);
  time->tm_mday = (int)ReadReg(DYMO);
  time->tm_mon = (int)ReadReg(MNTH) - 1;
  time->tm_wday = (int)ReadReg(DYWK) % 7;
  time->tm_yday = (int)ReadReg(DYYR) - 1;
  
  // YEAR represents a year between 0-99 while CENT represents a century
  // between 0-99
  time->tm_year = (100 * (int)ReadReg(CENT) + (int)ReadReg(YEAR)) - 1900;
}
