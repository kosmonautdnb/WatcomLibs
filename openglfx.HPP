#ifndef __OPENGLFX_HPP__
#define __OPENGLFX_HPP__

#define glFXAA() doFXAA(glFrameBuffer, glFrameBufferWidth, glFrameBufferHeight)
#define glToonShader() toonShader(glFrameBuffer,glFrameBufferWidth,glFrameBufferHeight);
#define glMotionBlur() doMotionBlur(glFrameBuffer)
#define glInitMotionBlur() allocMotionBlur(glFrameBufferWidth,glFrameBufferHeight)
#define glCloseMotionBlur() freeMotionBlur()

// Post processing Anti Aliasing (FXAA) // from glsl at geeks3d.com
// currently much too slow for realtime
//---------

void doFXAA(unsigned int *sourceBuffer, const int width, const int height);

// a simple toon shader..
// use glExplicitAlpha() for objectId (for Outlines) like glExplicitAlpha(true,objectId/255.f);
// if you don't use glExplicitAlpha() before drawing any object you get a crumbled alpha channel of the picture and thus crumbled output instead of outlines

extern float toonSaturation; // 1 = original color * toonColors, 0 = direct toonColors
extern float toonLuminance; // multiplies luminances before toon color lookup
extern float toonBaseLevel; // brightens up the luminances before toon color lookup
extern int toonColorCount; // used by toonColors->array[toonColorCount]
extern bool toonLuminances; // use toon Colors?
extern bool toonOutline; // have outlines?
extern bool toonTwoPixelOutlineX;  // outline two pixels in X
extern bool toonTwoPixelOutlineY; // outline two pixels in Y
extern unsigned int toonOutlineColor; // the color of the toon outlines
extern unsigned int *toonColors; // array[toonColorCount] of toon colors to be multiplied
extern float nonToonAlpha; // glExplicitAlpha() used for non toon textures
void toonShader(unsigned int *buffer, int width, int height);

// Frame rate dependend motion blur..

extern double motionBlurStrength;
void allocMotionBlur(unsigned int width, unsigned int height);
void freeMotionBlur();
void doMotionBlur(unsigned int *frameBuffer);

#endif //__OPENGLFX_HPP__
