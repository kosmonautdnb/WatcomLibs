#include "keymtrix.hpp"
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef __WATCOMC__
#include <i86.h>
#include <dos.h>
#endif // __WATCOMC__
#ifdef __DJGPP__
// todo: not implemented, yet
#include <dos.h>
#endif // __DJGPP__

#ifdef __WATCOMC__
typedef void (__interrupt __far* KeyboardHandler)();
static KeyboardHandler oldKeyboardHandler = NULL;

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
  oldKeyboardHandler = _dos_getvect(0x09);
  _dos_setvect(0x09,keyboardHandler);
  atexit(uninstallKeyboardHandler);
}

void uninstallKeyboardHandler() {
  if (oldKeyboardHandler != NULL) {
    _dos_setvect(0x09,oldKeyboardHandler);
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
