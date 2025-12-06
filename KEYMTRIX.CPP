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
#include <dos.h>
#include <dpmi.h>
#include <go32.h>
#endif // __DJGPP__

volatile bool keyPressed[0x80] = {0};

bool isKeyPressed(int scanCode) {
  return keyPressed[scanCode & 0x7f];
}

#ifdef __WATCOMC__

typedef void (__interrupt __far* KeyboardHandler)();
static KeyboardHandler oldKeyboardHandler = NULL;

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

#endif // __WATCOMC__

#ifdef __DJGPP__

#define LOCK_VARIABLE(x) _go32_dpmi_lock_data((void*)&x,(long)sizeof(x));
#define LOCK_FUNCTION(x,__s__) _go32_dpmi_lock_code((void*)x,(long)__s__);

static _go32_dpmi_seginfo old_keyboard_handler;
static _go32_dpmi_seginfo new_keyboard_handler;
static bool keyboard_handler_installed = false;

static void keyboardHandler() {
  unsigned char key = (unsigned char)inp(0x60);
  keyPressed[key & 0x7f] = key & 0x80 ? false : true;
}

void uninstallKeyboardHandler() {
  if (keyboard_handler_installed) {
    keyboard_handler_installed = false;
    _go32_dpmi_set_protected_mode_interrupt_vector(0x09,&old_keyboard_handler);
  }
}

void installKeyboardHandler() {
  if (keyboard_handler_installed) 
    uninstallKeyboardHandler();
  _go32_dpmi_get_protected_mode_interrupt_vector(0x09,&old_keyboard_handler); 
  keyboard_handler_installed = true;
  new_keyboard_handler.pm_offset = (unsigned long)keyboardHandler;
  new_keyboard_handler.pm_selector = _go32_my_cs();
  _go32_dpmi_chain_protected_mode_interrupt_vector(0x09,&new_keyboard_handler);
  LOCK_FUNCTION(keyboardHandler,100);
  LOCK_VARIABLE(keyPressed);
}

#endif // __DJGPP__

