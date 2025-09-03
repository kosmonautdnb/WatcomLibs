#ifndef __OPENGLFX_HPP__
#define __OPENGLFX_HPP__

#define glFXAA() doFXAA(glFrameBuffer, glFrameBufferWidth, glFrameBufferHeight)
#define glToonShader() toonShader(glFrameBuffer,glDepthBuffer,glFrameBufferWidth,glFrameBufferHeight);
#define glInitMotionBlur() allocMotionBlur(glFrameBufferWidth,glFrameBufferHeight)
#define glCloseMotionBlur() freeMotionBlur()
#define glMotionBlur() doMotionBlur(glFrameBuffer)
#define glSkySphere(__texture_atan2__) doSkySphere((__texture_atan2__),glDepthBuffer,glFrameBuffer,glFrameBufferWidth,glFrameBufferHeight);
#define glSkyPlane(__texture__) doSkyPlane(__texture__,glDepthBuffer,glFrameBuffer,glFrameBufferWidth,glFrameBufferHeight);
#define glBloom(__passes__,__threshold__,__gain__) bloom(glFrameBuffer,glFrameBufferWidth,glFrameBufferHeight,__passes__,__threshold__,__gain__);

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
extern bool toonUseDepthBuffer; // so it looks like outlines even with stuff in front
extern bool toonDepthInvert; // sometimes needed
void toonShader(unsigned int *buffer, float *depth, int width, int height);

// Frame rate dependend motion blur..

extern double motionBlurStrength;
void allocMotionBlur(unsigned int width, unsigned int height);
void freeMotionBlur();
void doMotionBlur(unsigned int *frameBuffer);

// Sky Sphere, much like a skybox but with a panorama texture (same format to GL_SPHERE_MAP_ATAN2)

void doSkySphere(unsigned int texture_, float *zBuffer, unsigned int *frameBuffer, unsigned int width, unsigned int height);

extern float _sky_z_value;  // 1.f
extern float _sky_sphere_focus_plane; // between -1 and 1 (field of view)
extern float _sky_sphere_movement; // rotating the skybox on the y axis
extern bool _sky_sphere_yflipped; // upside down
extern bool _sky_sphere_xflipped;
extern int _sky_sphere_stepWidth; // how fine granular the calculations are 16 is a good value the bigger the better the performance but less quality

// A sky plane with two textures moving.. For clouds..

void doSkyPlane(unsigned int texture_, float *zBuffer, unsigned int *frameBuffer, unsigned int width, unsigned int height);

extern float _sky_plane_movex_0;
extern float _sky_plane_movez_0;
extern float _sky_plane_scale_0;
extern float _sky_plane_movex_1;
extern float _sky_plane_movez_1;
extern float _sky_plane_scale_1;
extern float _sky_plane_focus_plane; // between -1 and 1 (field of view)
extern int _sky_plane_stepWidth; // how fine granular the calculations are 16 is a good value the bigger the better the performance but less quality

// A gaussian bloom
void bloom(unsigned int *frameBuffer, unsigned int width, unsigned int height, int passes, float threshold, float gain);

#endif //__OPENGLFX_HPP__
