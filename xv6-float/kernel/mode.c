#include <stddef.h>

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "mode.h"

struct mode_entry modes[NMODE];
struct perm_entry perm_entries[NMODE];

struct mlb_lru mlb_lru[LBSIZE-1];
struct plb_lru plb_lru[LBSIZE-1];

struct mlb_lru *least_mlb_elem;
struct plb_lru *least_plb_elem;
struct mlb_lru *most_mlb_elem;
struct plb_lru *most_plb_elem;

 void plb_set_size_perm(struct perm_entry *entry, uint64 size, uint64 perm)
 {
     perm = perm << 45;
     size = size | perm;
     memmove(entry->size_perm, &size, 6);
 }
 
 uint64 plb_get_perm(struct perm_entry *entry)
 {
     uint64 perm = 0;
     memmove(&perm, entry->size_perm, 6);
     return perm >> 45;
 }
 
 uint64 plb_get_size(struct perm_entry *entry)
 {
     uint64 size = 0;
     memmove(&size, entry->size_perm, 6);
     uint64 perm_mask = ~(0x7ULL << 45);
     size = size & perm_mask;
     return size;
 }

struct mode_entry *get_entry_for_modeid(uint16 modeid)
{
  return &modes[modeid - 1];
}

struct mlb_lru* get_lru_mlb_line()
{
  struct mlb_lru *next_least_mlb = least_mlb_elem->next;
  most_mlb_elem->next = least_mlb_elem;
  most_mlb_elem = least_mlb_elem;
  most_mlb_elem->next = NULL;
  least_mlb_elem = next_least_mlb;
  return most_mlb_elem;
}

struct plb_lru* get_lru_plb_line()
{
  struct plb_lru *next_least_plb = least_plb_elem->next;
  most_plb_elem->next = least_plb_elem;
  most_plb_elem = least_plb_elem;
  most_plb_elem->next = NULL;
  least_plb_elem = next_least_plb;
  return most_plb_elem;
}

struct plb_lru* get_plb_line_for_perm_entry(struct perm_entry *entry)
{
  struct plb_lru *elem;

  for(elem = least_plb_elem; elem != NULL; elem = elem->next)
  {
    if(entry == elem->entry)
    {
      return elem;
    }
  }
  return NULL;
}
struct mlb_lru* get_mlb_line_for_modeid(uint16 modeid)
{
  struct mlb_lru* elem;
  for(elem = least_mlb_elem; elem != NULL; elem = elem->next)
  {
    if(elem->entry != NULL && elem->entry->modeid == modeid)
    {
      return elem;
    }
  }
  return NULL;
}

int store_perm(struct perm_entry *perm)
{
  struct plb_lru *elem = get_plb_line_for_perm_entry(perm);
  if(elem == NULL)
  {
    elem = get_lru_plb_line();
  }

  elem->entry = perm;
  int line = elem->line;
  plbstore(line, (uint64)perm);
  return 0;
}
int store_mode(uint16 modeid)
{
  int line;
  struct mode_entry *entry = get_entry_for_modeid(modeid);
  if(entry->modeid == 0)
  {
    return -1;
  }

  if(modeid == 1)
  {
    line = 0;
  }
  else
  {
  
  struct mlb_lru *elem = get_mlb_line_for_modeid(modeid);
  if(elem == NULL)
  {
    elem = get_lru_mlb_line();
  }

  elem->entry = entry;
  line = elem->line;
  }
  mlbstore(line, (uint64)entry);

  return 0;
}

void invalidate_perm(struct perm_entry *perm)
{
  struct plb_lru *elem = get_plb_line_for_perm_entry(perm);
  if(elem == NULL)
  {
    return;
  }

  elem->entry = NULL;

  struct perm_entry entry;
  memset(&entry, 0, sizeof(struct perm_entry));
  plbstore(elem->line, (uint64)&entry);
}
void invalidate_mode(uint16 modeid)
{
  if(modeid == 1)
  {
    panic("trying to invalidate kernel mlb!");
  }
  struct mlb_lru *elem = get_mlb_line_for_modeid(modeid);
  if(elem == NULL)
  {
    return;
  }

  elem->entry = NULL;

  struct mode_entry entry;
  memset(&entry, 0, sizeof(struct mode_entry));
  mlbstore(elem->line, (uint64)&entry);
}
void modeinit()
{
  //Entry 0 contains the kernel. NEVER EVICT THAT!
  int i;
     for(i = 0; i < LBSIZE -2; i++)
     {
       mlb_lru[i].line = i+1;
       mlb_lru[i].next = &mlb_lru[i+1];
       plb_lru[i].line = i+1;
       plb_lru[i].next = &plb_lru[i+1];
     }

     mlb_lru[i].line = i+1;
     mlb_lru[i].next = NULL;
     plb_lru[i].line = i+1;
     plb_lru[i].next = NULL;

     least_mlb_elem = &mlb_lru[0];
     least_plb_elem = &plb_lru[0];
     most_mlb_elem = &mlb_lru[i];
     most_plb_elem = &plb_lru[i];

     perm_entries[0].modeid = 1;
     perm_entries[0].start = 0;
     plb_set_size_perm(&perm_entries[0],0xffffffff, PLB_R | PLB_W | PLB_X);
}

int permalloc(uint16 modeid, uint64 start, uint64 end, uint16 perm)
{
  int i;
  for(i = 0; i < NMODE; i++)
  {
    if(perm_entries[i].modeid == 0)
      break;
  }
  if(i == NMODE)
  {
    return -1;
  }
   perm_entries[i].start = start;
   perm_entries[i].modeid = modeid;
   plb_set_size_perm(&perm_entries[i], end, perm);

   store_perm(&perm_entries[i]);
   return 0;
}

//Creates a child mode 
int modealloc(uint16 parent)
{
  int i;
  for(i = 0;i < NMODE; i++)
  {
    if(modes[i].modeid == parent)
    {
      break;
    }
  }

  if(i == NMODE)
  {
    return -1;
  }

  for(i = 0; i < NMODE; i++)
  {
    if(modes[i].modeid == 0)
    {
      break;
    }
  }

  if(i == NMODE)
  {
    panic("exhausted id space");
  }

  //Modes start with 1, array start with 0
  modes[i].modeid = i + 1;
  modes[i].epc = 0;
  modes[i].entry_point = 0;
  modes[i].parent = parent;
  modes[i].bits = 0;

  int j;
  for(j = 0;j < NMODE; j++)
  {
    if(perm_entries[j].modeid == parent)
    {
      permalloc(i+1, perm_entries[j].start, plb_get_size(&perm_entries[j]), plb_get_perm(&perm_entries[j]));
    }
  }
  store_mode(i + 1);
  return i+1;
}




void rec_free(struct mode_entry *parent_entry)
{
  int i = 0;
  for(; i < NMODE; i++)
  {
    if(modes[i].parent == parent_entry->modeid)
    {
      rec_free(&modes[i]);
    }
  }

     int j = 0;
     for(; j < NMODE; j++)
     {
       if(perm_entries[j].modeid == parent_entry->modeid)
       {
         invalidate_perm(&perm_entries[j]);
         memset(&perm_entries[j], 0, sizeof(struct perm_entry));
       }
     }
     invalidate_mode(parent_entry->modeid);
     memset(parent_entry, 0, sizeof(struct mode_entry));
}

int mode_permchange(uint16 mode, uint64 start, uint64 size, uint64 perm)
{
  int i;
  uint64 prev_size = 0;
  uint64 prev_perm = 0;
  for(i = 0; i < NMODE; i++)
  {
    if(perm_entries[i].modeid == mode)
    {
      prev_size = plb_get_size(&perm_entries[i]);
      prev_perm = plb_get_perm(&perm_entries[i]);
      if(perm_entries[i].start <= start && perm_entries[i].start + prev_size >= start + size)
      {
        if( (prev_perm | perm) == prev_perm)
        {
          break;
        }
      }
    }
  }
    if(i == NMODE)
    {
      return -1;
    }

    if(perm_entries[i].start < start)
    {
      permalloc(perm_entries[i].modeid, perm_entries[i].start, start - perm_entries[i].start, prev_perm);
    }

    if(perm_entries[i].start + prev_size > start + size)
    {
      permalloc(perm_entries[i].modeid, start + size, prev_size - (start - perm_entries[i].start)  - size, prev_perm);
    }

    permalloc(mode, start, size, perm);

  invalidate_perm(&perm_entries[i]);
  memset(&perm_entries[i], 0, sizeof(struct perm_entry));
  return 0;
}
int modefree(uint16 modeid)
{
  struct mode_entry *entry = get_entry_for_modeid(modeid);
  rec_free(entry);
  return 0;
}

int mode_mcntl(uint16 modeid, uint64 opt)
{
  if(opt & SET_FI && opt & UNSET_FI)
  {
    return -1;
  }
  if(opt & SET_SI && opt & UNSET_SI)
  {
    return -1;
  }
  
  struct mode_entry *entry = get_entry_for_modeid(modeid);
  if(entry->modeid == 0)
  {
    return -1;
  }
  if(opt & SET_FI)
  {
    entry->bits |= 0x1;
  }
  else if(opt & UNSET_FI)
  {
    entry->bits &= ~(0x1);
  }
  if(opt & SET_FI)
  {
    entry->bits |= 0x1;
  }
  else if(opt & UNSET_FI)
  {
    entry->bits &= ~(0x1);
  }

  return store_mode(modeid);
}
int mode_set_entry_point(uint16 modeid, uint64 entry_point)
{
  struct mode_entry *entry =  get_entry_for_modeid(modeid);

  if(entry->modeid == 0)
  {
    return -1;
  }

  entry->entry_point = entry_point;
  return store_mode(modeid);
}
int mode_set_epc(uint16 modeid, uint64 epc)
{
  struct mode_entry *entry =  get_entry_for_modeid(modeid);

  if(entry->modeid == 0)
  {
    return -1;
  }

  entry->epc = epc;
  return store_mode(modeid);
}
