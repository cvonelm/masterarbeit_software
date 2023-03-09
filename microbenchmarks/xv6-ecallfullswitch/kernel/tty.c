//
// low-level driver routines for LupIO TTY.
//

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

// LupIO TTY is a memory mapped device. This macro returns the address of one
// of the registers.
#define Reg(reg) ((volatile unsigned int *)(LUPIO_TTY + reg))

// The offsets for the registers.
#define WRIT 0x0
#define READ 0x4
#define CTRL 0x8
#define STAT 0xC

// masks for the CTRL and STAT registers
#define WBIT 1
#define RBIT (1 << 1)

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

// the transmit output buffer.
struct spinlock tty_tx_lock;
#define TTY_TX_BUF_SIZE 32
char tty_tx_buf[TTY_TX_BUF_SIZE];
uint64 tty_tx_w; // write next to tty_tx_buf[tty_tx_w % TTY_TX_BUF_SIZE]
uint64 tty_tx_r; // read next from tty_tx_buf[tty_tx_r % TTY_TX_BUF_SIZE]

extern volatile int panicked; // from printf.c

void ttystart();

void
ttyinit(void)
{
  // enable transmit and receive interrupts
  WriteReg(CTRL, WBIT | RBIT);

  initlock(&tty_tx_lock, "tty");
}

void
ttyputc(int c)
{
  acquire(&tty_tx_lock);

  if(panicked){
    for(;;)
      ;
  }

  while(1){
    if(tty_tx_w == tty_tx_r + TTY_TX_BUF_SIZE){
      // buffer is full.
      // wait for ttystart() to open up space in the buffer.
      sleep(&tty_tx_r, &tty_tx_lock);
    } else {
      tty_tx_buf[tty_tx_w % TTY_TX_BUF_SIZE] = c;
      tty_tx_w += 1;
      ttystart();
      release(&tty_tx_lock);
      return;
    }
  }
}

void
ttyputc_sync(int c)
{
  push_off();

  if(panicked){
    for(;;)
      ;
  }

  // wait until WBIT is set in STAT.
  while((ReadReg(STAT) & WBIT) == 0)
    ;
  WriteReg(WRIT, c);

  pop_off();
}

void
ttystart(void)
{
  while(1){
    if(tty_tx_w == tty_tx_r){
      // transmit buffer is empty.
      return;
    }

    if((ReadReg(STAT) & WBIT) == 0){
      // the tty transmit holding register is full,
      // so we cannot give it another byte.
      // it will interrupt when it's ready for a new byte.
      return;
    }

    int c = tty_tx_buf[tty_tx_r % TTY_TX_BUF_SIZE];
    tty_tx_r += 1;

    // maybe ttyputc() is waiting for space in the buffer.
    wakeup(&tty_tx_r);

    WriteReg(WRIT, c);
  }
}

// read one input character from tty
// return -1 if none is waiting
int
ttygetc(void)
{
  if(ReadReg(STAT) & RBIT){
    // input data is ready
    return ReadReg(READ);
  } else {
    return -1;
  }
}

void
ttyintr(void)
{
  // read and process incoming characters
  while(1){
    int c = ttygetc();
    if(c == -1){
      break;
    }
    consoleintr(c);
  }

  // send buffered characters
  acquire(&tty_tx_lock);
  // lowers transmitter irq
  ReadReg(WRIT);
  ttystart();
  release(&tty_tx_lock);
}
