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

static int key0xe0 = 0;
static int key0xe0val = 0;
static void keyboardHandler() {
  unsigned char key = (unsigned char)inp(0x60);
  if (key == 0xe0) {
    key0xe0 = 2; // extended keyboard message tag
  }
  if (key0xe0 == 2 && key != 0xe0) {key0xe0 = 1; key0xe0val = key;} // not the extended message tag anymore but the message value
  if (key0xe0 == 1 && key != key0xe0val) key0xe0 = 0x00; // not the extended value anymore
  if (key0xe0 == 1) { // some extended messages belong to special keys here
    switch(key & 0x7f) {
    case SCANCODE_LEFT:
    case SCANCODE_RIGHT:
    case SCANCODE_UP:
    case SCANCODE_DOWN:
    case SCANCODE_WINDOWS:
      {key0xe0 = 0;} break; // clear extended message handling here
    case SCANCODE_LALT: {key = SCANCODE_RALT|(key&0X80); keyPressed[key & 0x7f] = key & 0x80 ? false : true;} break;
    case SCANCODE_LCTRL: {key = SCANCODE_RCTRL|(key&0X80); keyPressed[key & 0x7f] = key & 0x80 ? false : true;} break;
    }
  }
  if (key0xe0 == 0) {
    keyPressed[key & 0x7f] = key & 0x80 ? false : true;
  }
}


#ifdef __WATCOMC__

typedef void (__interrupt __far* KeyboardHandler)();
static KeyboardHandler oldKeyboardHandler = NULL;

static void (__interrupt __far newKeyboardHandler) (void) {
  keyboardHandler();
  oldKeyboardHandler();
}                                               

void installKeyboardHandler() {
  if (oldKeyboardHandler != NULL)
    uninstallKeyboardHandler();
  oldKeyboardHandler = _dos_getvect(0x09);
  _dos_setvect(0x09,newKeyboardHandler);
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

