#include "fxmblur.hpp"
#include <stdlib.h>
#include <string.h>

unsigned int *_motionBlur = NULL;
unsigned int _motionBlurWidth = 0;
unsigned int _motionBlurHeight = 0;
double motionBlurStrength = 0.75; // between 0 and 1 (also frame rate dependent)

void allocMotionBlur(unsigned int width, unsigned int height) {
  if (_motionBlur != NULL) delete[] _motionBlur;
  _motionBlur = new unsigned int[width*height];
  _motionBlurWidth = width;
  _motionBlurHeight = height;
  memset(_motionBlur,0,width*height*sizeof(unsigned int));
}

void freeMotionBlur() {
  if (_motionBlur != NULL) delete[] _motionBlur;
  _motionBlur = NULL;
  _motionBlurWidth = 0;
  _motionBlurHeight = 0;
}

void doMotionBlur(unsigned int *frameBuffer) {
  unsigned int *sourceH = frameBuffer; 
  unsigned int *sourceL = _motionBlur; 
  const unsigned int siz = _motionBlurWidth * _motionBlurHeight;
  int p = (int)(0x100*(1.f-motionBlurStrength));
  int n = 256-p;
  for (int i = 0; i < siz; i++) {
    const unsigned int here = *sourceH;
    const unsigned int last = *sourceL;
    const unsigned int h0 = ((here & 0x00ff00ff)*p+(last & 0x00ff00ff)*n) & 0xff00ff00;
    const unsigned int h1 = ((here & 0x0000ff00)*p+(last & 0x0000ff00)*n) & 0x00ff0000;
    const unsigned int h = (h0>>8)|(h1>>8)|0xff000000;
    *sourceH++ = h;
    *sourceL++ = h;
  }
}
