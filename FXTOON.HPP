// a simple toon shader..
// use glExplicitAlpha() for objectId (for Outlines) like glExplicitAlpha(true,objectId/255.f);
// if you don't use glExplicitAlpha() before drawing any object you get a crumbled alpha channel of the picture and thus crumbled output instead of outlines
#ifndef __FXTOON_HPP__
#define __FXTOON_HPP__

#define glToonShader() toonShader(glFrameBuffer,glFrameBufferWidth,glFrameBufferHeight);

extern float toonSaturation; // 1 = original color * toonColors, 0 = direct toonColors
extern float toonLuminance;
extern float toonBaseLevel;
extern int toonColorCount;
extern bool toonLuminances;
extern bool toonOutline;
extern bool toonTwoPixelOutlineX;
extern bool toonTwoPixelOutlineY;
extern unsigned int toonOutlineColor;
extern unsigned int *toonColors;

void toonShader(unsigned int *buffer, int width, int height);

#endif //__FXTOON_HPP__
