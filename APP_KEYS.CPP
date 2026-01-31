/*
  Just a simple keypress logger.
*/
/*
  MIT License

  Copyright (c) 2026 Stefan Mader

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
// WatcomGL can be found here: https://github.com/kosmonautdnb/WatcomGL
// Some additional Libraries for WatcomC can be found here: https://github.com/kosmonautdnb/WatcomLibs
#include "GL.H"
#include "KEYMTRIX.HPP"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

int main(int argc, char *argv[]) {
  int key = 0;
  printf("Press a key:\n");
  while(1) {
    key = glNextKey();
    GLboolean shift,ctrl,alt;
    glSpecialKeys(&shift,&ctrl,&alt);
    if (shift != 0x00) printf("shift;");
    if (ctrl != 0x00) printf("ctrl;");
    if (alt != 0x00) printf("alt;");
    if (key != 0) break;
  }
  printf("WatcomGL keycode:%d ($%04x)\n", key, key);

  while(1) {
    key = (unsigned char)inp(0x60);
    if (key & 0x80) break;
  }

  printf("Press another key:\n");
  while(1) {
    key = (unsigned char)inp(0x60);
    if ((key & 0x80) == 0) {
      printf("Scancode: $%02x(%d)\n", key, key);
      break;
    }
  }

  while(glNextKey()!=0) {;}

  printf("And press another key:\n");
  installKeyboardHandler();
  for (int i = 0; i < KEYMATRIX_KEYCOUNT; i++) if (isKeyPressed(i)) {i=0;}
  key = -1;
  while(1) {
    for (int i = 0; i < KEYMATRIX_KEYCOUNT; i++) if (isKeyPressed(i)) {key = i; break;}
    if (key != -1) break;
  }
  printf("Keymatrix ID here: $%02x(%d)\n", key, key);
  uninstallKeyboardHandler();
  while(glNextKey()!=0) {;}
  return 0;
}
