#ifndef __FXMBLUR_HPP__
#define __FXMBLUR_HPP__

#define glMotionBlur() doMotionBlur(glFrameBuffer)
#define glInitMotionBlur() allocMotionBlur(glFrameBufferWidth,glFrameBufferHeight)
#define glCloseMotionBlur() freeMotionBlur()

extern double motionBlurStrength;

void allocMotionBlur(unsigned int width, unsigned int height);
void freeMotionBlur();
void doMotionBlur(unsigned int *frameBuffer);

#endif //__FXMBLUR_HPP__
