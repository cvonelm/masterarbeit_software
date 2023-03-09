#include "types.h"         


#define PLB_R 0x1
#define PLB_W 0x2
#define PLB_X 0x4

#define MLB_SI 0x2
#define MLB_FI 0x1

#define SET_FI 0x8
#define UNSET_FI 0x4
#define SET_SI 0x2
#define UNSET_SI 0x1

struct __attribute__((packed)) perm_entry
          {
              uint16 modeid;
              uint64 start;
              char size_perm[6];
          };
 struct __attribute__((packed)) mode_entry
 {
   uint64 entry_point;
   uint64 epc;
   uint16 modeid;
   uint16 parent;
   uint32 bits;
 };
 
struct mlb_lru
{
  uint16 line;
  struct mode_entry *entry;
  struct mlb_lru *next;
};

struct plb_lru
{
  uint16 line;
  struct perm_entry *entry;
  struct plb_lru *next;
};
