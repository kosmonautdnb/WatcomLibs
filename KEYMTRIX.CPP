#include "keymtrix.hpp"
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef __WATCOMC__
#include <i86.h>
#endif // __WATCOMC__
#ifdef __DJGPP__
// todo: not implemented, yet
#include <dos.h>
#endif // __DJGPP__

// this is not working in dos4gw, in !!!!!pmode/w!!!!! it's working

#ifdef __WATCOMC__
typedef void (__interrupt __far* KeyboardHandler)();
static KeyboardHandler oldKeyboardHandler = NULL;

#define DPMIERROR {printf("DPMI Error. No appropiate DPMI interface. Maybe use PModeW at linking.\n");exit(0);}

bool keyPressed[0x80] = {0};

bool isKeyPressed(int scanCode) {
  return keyPressed[scanCode & 0x7f];
}

static void (__interrupt __far keyboardHandler) (void) {
  unsigned char key = inp(0x60);
  keyPressed[key & 0x7f] = key & 0x80 ? false : true;
  oldKeyboardHandler();
}

void installKeyboardHandler() {
  if (oldKeyboardHandler != NULL)
    uninstallKeyboardHandler();
  union REGS r;
  memset(&r,0,sizeof(r));
  r.w.ax = 0x204;
  r.h.bl = 0x09;
  int386 (0x31, &r, &r);
  oldKeyboardHandler = (KeyboardHandler)MK_FP(r.x.ecx,r.x.edx);
  memset(&r,0,sizeof(r));
  r.w.ax = 0x205;
  r.h.bl = 0x09;
  r.x.ecx = FP_SEG(keyboardHandler);
  r.x.edx = FP_OFF(keyboardHandler);
  int386 (0x31, &r, &r);
  if (r.w.cflag) DPMIERROR;
  atexit(uninstallKeyboardHandler);
}

void uninstallKeyboardHandler() {
  if (oldKeyboardHandler != NULL) {
    union REGS r;
    memset(&r,0,sizeof(r));
    r.w.ax = 0x205;
    r.h.bl = 0x09;
    r.x.ecx = FP_SEG(oldKeyboardHandler);
    r.x.edx = FP_OFF(oldKeyboardHandler);
    int386 (0x31, &r, &r);
    if (r.w.cflag) DPMIERROR;
    oldKeyboardHandler = NULL;
  }
}
#endif

#ifdef __DJGPP__

bool keyPressed[0x80] = {0};

bool isKeyPressed(int scanCode) {
  return keyPressed[scanCode & 0x7f];
}


void installKeyboardHandler() {
}

void uninstallKeyboardHandler() {
}

#endif
