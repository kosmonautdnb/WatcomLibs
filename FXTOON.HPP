// a simple toon shader..
// use glExplicitAlpha() for objectId (for Outlines) like glExplicitAlpha(true,objectId/255.f);
// if you don't use glExplicitAlpha() before drawing any object you get a crumbled alpha channel of the picture and thus crumbled output instead of outlines
#ifndef __FXTOON_HPP__
#define __FXTOON_HPP__

#define glToonShader() toonShader(glFrameBuffer,glFrameBufferWidth,glFrameBufferHeight);

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

#endif //__FXTOON_HPP__
