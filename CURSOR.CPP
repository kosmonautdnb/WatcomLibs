#include "CURSOR.HPP"
#include <string.h>
#ifdef __WATCOMC__
#include <i86.h>
#endif //__WATCOMC__
#ifdef __DJGPP__
#include <dos.h>
#endif //__DJGPP__

void getBiosCursorPosition(unsigned char *x, unsigned char *y) {
  union REGS regs;
  memset(&regs,0,sizeof(regs));
  regs.w.ax = 0x0300;
  regs.h.bh = 0;
  int386(0x10, &regs, &regs);
  if (x != NULL) *x = regs.h.dl;
  if (y != NULL) *y = regs.h.dh;
}

void setBiosCursorPosition(unsigned char x, unsigned char y) {
  union REGS regs;
  memset(&regs,0,sizeof(regs));
  regs.w.ax = 0x0200;
  regs.h.bh = 0;
  regs.h.dl = x;
  regs.h.dh = y;
  int386(0x10, &regs, &regs);
}
