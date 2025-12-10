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

#define NONSTANDARDKEY(__h__) {key = (__h__)|(key&0X80); keyPressed[key & 0x7f] = key & 0x80 ? false : true;} break;
static int key0xe0 = 0;
static int key0xe0val = 0;
static bool keyWasPause = false;
static void keyboardHandler() {
  unsigned char key = (unsigned char)inp(0x60);
  if (keyWasPause) {keyWasPause = false; keyPressed[SCANCODE_PAUSE] = false;} // no better workaround currently :/
  // e1 handling
  if (key == 0xe1)  key0xe0 = 4;
  if (key0xe0 == 4 && key != 0xe1) {key0xe0 = 3; key0xe0val = key;}
  if (key0xe0 == 3 && key != key0xe0val) key0xe0 = 0x00;
  if (key0xe0 == 3) {
    switch(key & 0x7f) {
    case 0x1d: NONSTANDARDKEY(SCANCODE_PAUSE)
    }
    if ((key & 0x7f) == SCANCODE_PAUSE) keyWasPause = true;
  }
  // e0 handling
  if (key == 0xe0) key0xe0 = 2; 
  if (key0xe0 == 2 && key != 0xe0) {key0xe0 = 1; key0xe0val = key;}
  if (key0xe0 == 1 && key != key0xe0val) key0xe0 = 0x00;
  if (key0xe0 == 1) {
    switch(key & 0x7f) {
    case 0x1c: NONSTANDARDKEY(SCANCODE_NUMENTER)
    case 0x35: NONSTANDARDKEY(SCANCODE_NUMDIVIDE)
    case 0x47: NONSTANDARDKEY(SCANCODE_START)
    case 0x48: NONSTANDARDKEY(SCANCODE_UP)
    case 0x49: NONSTANDARDKEY(SCANCODE_PAGEUP)
    case 0x4b: NONSTANDARDKEY(SCANCODE_LEFT)
    case 0x4d: NONSTANDARDKEY(SCANCODE_RIGHT)
    case 0x4f: NONSTANDARDKEY(SCANCODE_END)
    case 0x50: NONSTANDARDKEY(SCANCODE_DOWN)
    case 0x51: NONSTANDARDKEY(SCANCODE_PAGEDOWN)
    case 0x52: NONSTANDARDKEY(SCANCODE_INSERT)
    case 0x53: NONSTANDARDKEY(SCANCODE_DELETE)
    case 0x5b: NONSTANDARDKEY(SCANCODE_WINDOWS)
    case SCANCODE_LALT: NONSTANDARDKEY(SCANCODE_RALT)
    case SCANCODE_LCTRL: NONSTANDARDKEY(SCANCODE_RCTRL)
    }
  }
  // normal handling
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
  LOCK_FUNCTION(keyboardHandler,2048);
  LOCK_VARIABLE(keyPressed);
}

#endif // __DJGPP__

