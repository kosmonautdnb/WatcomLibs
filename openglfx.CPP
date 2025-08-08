#include "gl.h"
#include "openglfx.hpp"
#include <string.h>
#include <math.h>

#define PI 3.14159265358979323846

// ANTIALIASING

bool USE_FXAA = true;

#define OK (1.f/2.f)
float FXAA_REDUCE_MIN = 1.f/128.f;
float FXAA_REDUCE_MUL = 1.f/(8.f*OK);
float FXAA_SPAN_MAX = 8.f;

static __inline const float luma(const unsigned int rgba) {
//  return (float)((rgba & 255)+((rgba>>8) & 255)+((rgba>>16) & 255))*(1.f/255.f/3.f);
//  return (float)((rgba>>8) & 255)*(1.f/255.f);
  return (float)((rgba & 255)*25+(float)((rgba>>8) & 255)*60+(float)((rgba>>16) & 255)*15)*(1.f/255.f/100.f);
}

template<class T> 
static __inline const T min(const T &a, const T &b) {
  return a < b ? a : b;
}

template<class T> 
static __inline const T max(const T &a, const T &b) {
  return a > b ? a : b;
}

__inline unsigned int tex(unsigned int *source, const int width, const int height, const float tx, const float ty) {
  int itx = (int)(tx); // tx,ty not below 0 (trunc)
  int ity = (int)(ty);
  const float fx = tx - (float)itx;
  const float fy = ty - (float)ity;
  const int txf = (int)(fx * 256.0); // fx,fy not below 0 (trunc)
  const int tyf = (int)(fy * 256.0);
  if (itx<0) itx = 0;
  if (ity<0) ity = 0;
  if (itx>=width-1) itx = width-2;
  if (ity>=height-1) ity = height-2;
  const int p1v = ((256-txf)*(256-tyf))>>8; 
  const int p2v = ((txf)*(256-tyf))>>8; 
  const int p3v = ((txf)*(tyf))>>8; 
  const int p4v = ((256-txf)*(tyf))>>8;
  const unsigned int rgba00 = source[itx+ity*width];
  const unsigned int rgba10 = source[itx+1+ity*width];
  const unsigned int rgba01 = source[itx+(ity+1)*width];
  const unsigned int rgba11 = source[itx+1+(ity+1)*width];
  unsigned int rgba1 = (rgba00 & 0x0000ff00)*p1v;
  rgba1 += (rgba10 & 0x0000ff00)*p2v;
  rgba1 += (rgba11 & 0x0000ff00)*p3v;
  rgba1 += (rgba01 & 0x0000ff00)*p4v;
  unsigned int rgba2 = (rgba00 & 0x00ff00ff)*p1v;
  rgba2 += (rgba10 & 0x00ff00ff)*p2v;
  rgba2 += (rgba11 & 0x00ff00ff)*p3v;
  rgba2 += (rgba01 & 0x00ff00ff)*p4v;
  return ((rgba1 & 0x00ff0000)+(rgba2 & 0xff00ff00))>>8;
}

void doFXAA(unsigned int *sourceBuffer, const int width, const int height) {
  if (!USE_FXAA) 
    return;
  unsigned int *tempBuffer = sourceBuffer; // we don't need two buffers anymore
  float *tr = new float[width];

  float *k = tr;
  const float fy = 1;
  for (int x = 1; x < width - 1; x++) {
    const float fx = x;
    *k++=luma(tex(sourceBuffer,width,height,fx+0.5f,fy-0.5f));
  }
  for (int y = 1; y < height - 1; y++) {
    unsigned int *source = &sourceBuffer[y*width+1];
    unsigned int *dest = &tempBuffer[y*width+1];
    const float fy = y;
    const float fx = 1;
    float lumaNW = luma(tex(sourceBuffer,width,height,fx-0.5f,fy-0.5f));
    float lumaSW = luma(tex(sourceBuffer,width,height,fx-0.5f,fy+0.5f));
    k = tr;
    for (int x = 1; x < width - 1; x++) {
      const float fx = x;

      const float lumaM = luma(*source);
      const float lumaNE = *k;
      const float lumaSE = luma(tex(sourceBuffer,width,height,fx+0.5f,fy+0.5f));

      const float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW,lumaSE)));
      const float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW,lumaSE)));
      const float dx = -((lumaNW+lumaNE)-(lumaSW+lumaSE));
      const float dy = ((lumaNW+lumaSW)-(lumaNE+lumaSE));
      const float dirReduce = max((lumaNW+lumaNE+lumaSW+lumaSE)*(0.25f*FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
      const float rcpDirMin=1.0/(min(fabs(dx),fabs(dy))+dirReduce);
      const float dirx = min(FXAA_SPAN_MAX,max(-FXAA_SPAN_MAX,dx*rcpDirMin));
      const float diry = min(FXAA_SPAN_MAX,max(-FXAA_SPAN_MAX,dy*rcpDirMin));
  
      const float da1 = 1.0/3.0-0.5;
      const float da2 = 2.0/3.0-0.5;
      const float db1 = 0.0/3.0-0.5;
      const float db2 = 3.0/3.0-0.5;
      const unsigned int rgbA1 = tex(sourceBuffer,width,height,fx+dirx*da1,fy+diry*da1);
      const unsigned int rgbA2 = tex(sourceBuffer,width,height,fx+dirx*da2,fy+diry*da2);
      const unsigned int rgbB1 = tex(sourceBuffer,width,height,fx+dirx*db1,fy+diry*db1);
      const unsigned int rgbB2 = tex(sourceBuffer,width,height,fx+dirx*db2,fy+diry*db2);
      const unsigned int rgbA = ((rgbA1 & 0x00fefefe)+(rgbA2 & 0x00fefefe))>>1;
      const unsigned int rgbB = ((rgbA & 0x00fefefe)>>1)+(((rgbB1 & 0x00fcfcfc)+(rgbB2 & 0x00fcfcfc))>>2);
      const float lumaB = luma(rgbB);
      if ((lumaB<lumaMin)||(lumaB>lumaMax)) {
        *dest = rgbA;
      } else {
        *dest = rgbB;
      }

      *k++ = lumaSE;
      lumaNW = lumaNE;
      lumaSW = lumaSE;
      source++;
      dest++;
    }
  }
  delete[] tr;
//  memcpy(sourceBuffer,tempBuffer,width*height*sizeof(unsigned int));
}

// TOONSHADER

float toonSaturation = 1.f; // 1 = original color * toonColors, 0 = direct toonColors
float toonLuminance = 1.0f;
float toonBaseLevel = 0.f;
int toonColorCount = 4;
bool toonLuminances = true;
bool toonOutline = true;
bool toonTwoPixelOutlineX = true;
bool toonTwoPixelOutlineY = true;
unsigned int toonOutlineColor = 0xff000000;
static unsigned int toonColorsTest[4]={
  0xff000000,
  0xff555555,
  0xffaaaaaa,
  0xffffffff,
};
unsigned int *toonColors = toonColorsTest;
float nonToonAlpha = 2.f;

void toonShader(unsigned int *buffer, int width, int height) {
  unsigned int *dest = buffer;
  const int toToonColorNr = (100*100*255/toonColorCount);
  const int toonAdd2 = (int)floor(toonBaseLevel*100)*(100*255);
  const int toonSaturation2 = (int)floor(toonSaturation*256.0);
  const int toonLuminance2 = (int)floor(toonLuminance*100.0);
  int *lastSpanA = new int[width];
  const int nonToonAlpha2 = nonToonAlpha*255;
  for (int x = 0; x < width; x++) lastSpanA[x] = (dest[x]>>24)&255;
  for (int y = 0; y < height; y++)  {
    int lastA = (*dest>>24) & 255;
    for (int x = 0; x < width; x++) {
      const unsigned int rgba = *dest;
      int r = rgba & 255;
      int g = (rgba>>8) & 255;
      int b = (rgba>>16) & 255;
      int a = (rgba>>24) & 255;
      bool outlineHere =  (a != lastA)||(a != lastSpanA[x]);
      if (outlineHere && toonOutline) {
        const unsigned int t2 = (toonOutlineColor & 0x00ffffff)|(a<<24);
        *dest = t2;
        if (toonTwoPixelOutlineX && x>0) *(dest-1) = t2;
        if (toonTwoPixelOutlineY && y>0) *(dest-width) = t2;
      } else if (toonLuminances && (a != nonToonAlpha2)) {
        int luminance = r*25+g*60+b*15; // 0..100 (100*255)
        int colorNumber = (luminance*toonLuminance2+toonAdd2)/toToonColorNr;
        if ((unsigned int)colorNumber >= toonColorCount) {
          if (colorNumber < 0) 
            colorNumber = 0;
          else
            colorNumber = toonColorCount-1;
        }
        const unsigned int toon = toonColors[colorNumber];
        r = r*256/(g+1);
        b = b*256/(g+1);
        g = 256;
        r = (((r - g) * toonSaturation2)>>8)+g;
        b = (((b - g) * toonSaturation2)>>8)+g;
        r = (r * (toon&255))>>8;
        g = (g * ((toon>>8)&255))>>8;
        b = (b * ((toon>>16)&255))>>8;
        if (r > 255) r = 255;
        if (g > 255) g = 255;
        if (b > 255) b = 255;
        *dest = r|(g<<8)|(b<<16)|(a<<24);
      }
      dest++;
      lastA = a;
      lastSpanA[x] = a;
    }
  }
  delete[] lastSpanA;
}

// MOTIONBLUR

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

// sky sphere

unsigned int _sky_sphere_rgba_to_replace = 0x00000000;
float _sky_sphere_focus_plane = 0; // between -1 and 1
float _sky_sphere_movement = 0;
bool _sky_sphere_yflipped = false;
bool _sky_sphere_xflipped = false;
int _sky_sphere_stepWidth = 16;
float _sky_z_value = 1.f;

#define SKY_ENCODE\
  float nx = nxp;\
  float ny = nyp;\
  float nz = nzp;\
  const float l = sqrt(nx*nx+ny*ny+nz*nz);\
  if (l>0) {\
    nx /= l;\
    ny /= l;\
    nz /= l;\
  }\
  float theta = atan2(nz,nx)+_sky_sphere_movement;\
  float phi = acos(-ny);\
  float tx = fmod(0.5f+theta*(0.5f/PI),1.f);\
  float ty = phi/PI;\
  if (_sky_sphere_xflipped) tx = 1.f-tx;\
  if (_sky_sphere_yflipped) ty = 1.f-ty;\
  nextTextureX = tx*textureWidth; \
  nextTextureY = ty*textureHeight; 

void doSkySphere(unsigned int texture_, float *zBuffer, unsigned int *frameBuffer, unsigned int width, unsigned int height) {
  unsigned int *texture = glGetTexturePointer(texture_);
  unsigned int textureWidth = glGetTextureWidth(texture_);
  unsigned int textureHeight = glGetTextureHeight(texture_);

  double modelView[16];
  double projection[16];
  int view[4];
  glGetDoublev(GL_MODELVIEW_MATRIX,modelView);
  glGetDoublev(GL_PROJECTION_MATRIX,projection);
  glGetIntegerv(GL_VIEWPORT, view);

  double midX,midY,midZ;
  double blX,blY,blZ;
  double brX,brY,brZ;
  double tlX,tlY,tlZ;
  double trX,trY,trZ;
  float xv = width/2;
  float yv = height/2;
  gluUnProject(width/2, height/2, -1, modelView, projection, view, &midX, &midY, &midZ);
  gluUnProject(width/2-xv, height/2-yv, _sky_sphere_focus_plane, modelView, projection, view, &blX, &blY, &blZ);
  gluUnProject(width/2+xv, height/2-yv, _sky_sphere_focus_plane, modelView, projection, view, &brX, &brY, &brZ);
  gluUnProject(width/2-xv, height/2+yv, _sky_sphere_focus_plane, modelView, projection, view, &tlX, &tlY, &tlZ);
  gluUnProject(width/2+xv, height/2+yv, _sky_sphere_focus_plane, modelView, projection, view, &trX, &trY, &trZ);

  float _00_nx = tlX-midX;
  float _00_ny = tlY-midY;
  float _00_nz = tlZ-midZ;
  float _01_nx = trX-midX;
  float _01_ny = trY-midY;
  float _01_nz = trZ-midZ;
  float _10_nx = blX-midX;
  float _10_ny = blY-midY;
  float _10_nz = blZ-midZ;
  float _11_nx = brX-midX;
  float _11_ny = brY-midY;
  float _11_nz = brZ-midZ;

  float l;
  l = sqrt(_00_nx*_00_nx+_00_ny*_00_ny+_00_nz*_00_nz); if (l>0) {_00_nx/=l;_00_ny/=l;_00_nz/=l;}
  l = sqrt(_01_nx*_01_nx+_01_ny*_01_ny+_01_nz*_01_nz); if (l>0) {_01_nx/=l;_01_ny/=l;_01_nz/=l;}
  l = sqrt(_11_nx*_11_nx+_11_ny*_11_ny+_11_nz*_11_nz); if (l>0) {_11_nx/=l;_11_ny/=l;_11_nz/=l;}
  l = sqrt(_10_nx*_10_nx+_10_ny*_10_ny+_10_nz*_10_nz); if (l>0) {_10_nx/=l;_10_ny/=l;_10_nz/=l;}

  float _0y_nxa = (_10_nx - _00_nx)/height;
  float _0y_nya = (_10_ny - _00_ny)/height;
  float _0y_nza = (_10_nz - _00_nz)/height;
  float _0y_nxp = _00_nx;
  float _0y_nyp = _00_ny;
  float _0y_nzp = _00_nz;

  float _1y_nxa = (_11_nx - _01_nx)/height;
  float _1y_nya = (_11_ny - _01_ny)/height;
  float _1y_nza = (_11_nz - _01_nz)/height;
  float _1y_nxp = _01_nx;
  float _1y_nyp = _01_ny;
  float _1y_nzp = _01_nz;

  for (int y = 0; y < height; y++) {
    float nxp = _0y_nxp;
    float nyp = _0y_nyp;
    float nzp = _0y_nzp;
    float nxa = (_1y_nxp-_0y_nxp)/width;
    float nya = (_1y_nyp-_0y_nyp)/width;
    float nza = (_1y_nzp-_0y_nzp)/width;
    int stepsXHere = 0;
    int nextXHere = 0;
    int k = 0;
    int kAdd = 0;
    int nextTextureX,nextTextureY,lastTextureX,lastTextureY;
    SKY_ENCODE
    for (int x = 0; x < width; x++) {
      if (x >= nextXHere) {
        stepsXHere = _sky_sphere_stepWidth;
        lastTextureX = nextTextureX;
        lastTextureY = nextTextureY;
        nextXHere += stepsXHere;
        nxp += nxa * stepsXHere;
        nyp += nya * stepsXHere;
        nzp += nza * stepsXHere;
        SKY_ENCODE
        if (fabs(nextTextureX-lastTextureX)>textureWidth/2||fabs(nextTextureY-lastTextureY)>textureHeight/2) {
          nextXHere -= stepsXHere;
          nxp -= nxa * stepsXHere;
          nyp -= nya * stepsXHere;
          nzp -= nza * stepsXHere;
          stepsXHere = 1;
          nextXHere += stepsXHere;
          nxp += nxa * stepsXHere;
          nyp += nya * stepsXHere;
          nzp += nza * stepsXHere;
          SKY_ENCODE
        }
        k = 0;
        kAdd = 0x10000/stepsXHere;
      }
      if (*zBuffer >= _sky_z_value) {
        int tx2 = (((nextTextureX-lastTextureX)*k)>>16)+lastTextureX;
        int ty2 = (((nextTextureY-lastTextureY)*k)>>16)+lastTextureY;
        *frameBuffer = texture[tx2+ty2*textureWidth];
      }
      k += kAdd;
      frameBuffer++;
      zBuffer++;
    }
    _0y_nxp += _0y_nxa;
    _0y_nyp += _0y_nya;
    _0y_nzp += _0y_nza;
    _1y_nxp += _1y_nxa;
    _1y_nyp += _1y_nya;
    _1y_nzp += _1y_nza;
  }
}

// sky plane

float _sky_plane_movex_0 = 0;
float _sky_plane_movez_0 = 0;
float _sky_plane_scale_0=0.1;
float _sky_plane_movex_1 = 0;
float _sky_plane_movez_1 = 0;
float _sky_plane_scale_1=0.075;
float _sky_plane_focus_plane = 0; // between -1 and 1
int _sky_plane_stepWidth = 16;

#define SKY_PLANE\
  float nx = nxp;\
  float ny = nyp;\
  float nz = nzp;\
  const float l = sqrt(nx*nx+ny*ny+nz*nz);\
  if (l>0) {\
    nx /= l;\
    ny /= l;\
    nz /= l;\
  }\
  if (nz!=0.f) {\
    nextTextureX = (nx/ny*_sky_plane_scale_0+_sky_plane_movex_0)*textureWidth; \
    nextTextureY = (nz/ny*_sky_plane_scale_0+_sky_plane_movez_0)*textureHeight;\
    if (nextTextureX < 0) \
      nextTextureX += (((-nextTextureX)+textureWidth-1)/textureWidth)*textureWidth;\
    else\
      nextTextureX %= textureWidth;\
    if (nextTextureY < 0) \
       nextTextureY += (((-nextTextureY)+textureHeight-1)/textureHeight)*textureHeight;\
    else\
      nextTextureY %= textureHeight;\
  }\
  if (nz!=0.f) {\
    nextTextureX2 = (nx/ny*_sky_plane_scale_1+_sky_plane_movex_1)*textureWidth; \
    nextTextureY2 = (nz/ny*_sky_plane_scale_1+_sky_plane_movez_1)*textureHeight;\
    if (nextTextureX2 < 0) \
      nextTextureX2 += (((-nextTextureX2)+textureWidth-1)/textureWidth)*textureWidth;\
    else\
      nextTextureX2 %= textureWidth;\
    if (nextTextureY2 < 0) \
       nextTextureY2 += (((-nextTextureY2)+textureHeight-1)/textureHeight)*textureHeight;\
    else\
      nextTextureY2 %= textureHeight;\
  } 

void doSkyPlane(unsigned int texture_, float *zBuffer, unsigned int *frameBuffer, unsigned int width, unsigned int height) {

  unsigned int *texture = glGetTexturePointer(texture_);
  unsigned int textureWidth = glGetTextureWidth(texture_);
  unsigned int textureHeight = glGetTextureHeight(texture_);

  double modelView[16];
  double projection[16];
  int view[4];
  glGetDoublev(GL_MODELVIEW_MATRIX,modelView);
  glGetDoublev(GL_PROJECTION_MATRIX,projection);
  glGetIntegerv(GL_VIEWPORT, view);

  double midX,midY,midZ;
  double blX,blY,blZ;
  double brX,brY,brZ;
  double tlX,tlY,tlZ;
  double trX,trY,trZ;
  float xv = width/2;
  float yv = height/2;
  gluUnProject(width/2, height/2, -1, modelView, projection, view, &midX, &midY, &midZ);
  gluUnProject(width/2-xv, height/2-yv, _sky_sphere_focus_plane, modelView, projection, view, &blX, &blY, &blZ);
  gluUnProject(width/2+xv, height/2-yv, _sky_sphere_focus_plane, modelView, projection, view, &brX, &brY, &brZ);
  gluUnProject(width/2-xv, height/2+yv, _sky_sphere_focus_plane, modelView, projection, view, &tlX, &tlY, &tlZ);
  gluUnProject(width/2+xv, height/2+yv, _sky_sphere_focus_plane, modelView, projection, view, &trX, &trY, &trZ);

  float _00_nx = tlX-midX;
  float _00_ny = tlY-midY;
  float _00_nz = tlZ-midZ;
  float _01_nx = trX-midX;
  float _01_ny = trY-midY;
  float _01_nz = trZ-midZ;
  float _10_nx = blX-midX;
  float _10_ny = blY-midY;
  float _10_nz = blZ-midZ;
  float _11_nx = brX-midX;
  float _11_ny = brY-midY;
  float _11_nz = brZ-midZ;

  float l;
  l = sqrt(_00_nx*_00_nx+_00_ny*_00_ny+_00_nz*_00_nz); if (l>0) {_00_nx/=l;_00_ny/=l;_00_nz/=l;}
  l = sqrt(_01_nx*_01_nx+_01_ny*_01_ny+_01_nz*_01_nz); if (l>0) {_01_nx/=l;_01_ny/=l;_01_nz/=l;}
  l = sqrt(_11_nx*_11_nx+_11_ny*_11_ny+_11_nz*_11_nz); if (l>0) {_11_nx/=l;_11_ny/=l;_11_nz/=l;}
  l = sqrt(_10_nx*_10_nx+_10_ny*_10_ny+_10_nz*_10_nz); if (l>0) {_10_nx/=l;_10_ny/=l;_10_nz/=l;}

  float _0y_nxa = (_10_nx - _00_nx)/height;
  float _0y_nya = (_10_ny - _00_ny)/height;
  float _0y_nza = (_10_nz - _00_nz)/height;
  float _0y_nxp = _00_nx;
  float _0y_nyp = _00_ny;
  float _0y_nzp = _00_nz;

  float _1y_nxa = (_11_nx - _01_nx)/height;
  float _1y_nya = (_11_ny - _01_ny)/height;
  float _1y_nza = (_11_nz - _01_nz)/height;
  float _1y_nxp = _01_nx;
  float _1y_nyp = _01_ny;
  float _1y_nzp = _01_nz;

  for (int y = 0; y < height; y++) {
    float nxp = _0y_nxp;
    float nyp = _0y_nyp;
    float nzp = _0y_nzp;
    float nxa = (_1y_nxp-_0y_nxp)/width;
    float nya = (_1y_nyp-_0y_nyp)/width;
    float nza = (_1y_nzp-_0y_nzp)/width;
    int stepsXHere = 0;
    int nextXHere = 0;
    int k = 0;
    int kAdd = 0;
    int nextTextureX,nextTextureY,lastTextureX,lastTextureY;
    int nextTextureX2,nextTextureY2,lastTextureX2,lastTextureY2;
    SKY_PLANE
    for (int x = 0; x < width; x++) {
      if (x >= nextXHere) {
        lastTextureX = nextTextureX;
        lastTextureY = nextTextureY;
        lastTextureX2 = nextTextureX2;
        lastTextureY2 = nextTextureY2;
        stepsXHere = _sky_sphere_stepWidth;
        nextXHere += stepsXHere;
        nxp += nxa * stepsXHere;
        nyp += nya * stepsXHere;
        nzp += nza * stepsXHere;
        SKY_PLANE
        if ((fabs(nextTextureX-lastTextureX)>textureWidth/2||fabs(nextTextureY-lastTextureY)>textureHeight/2) ||
          (fabs(nextTextureX2-lastTextureX2)>textureWidth/2||fabs(nextTextureY2-lastTextureY2)>textureHeight/2))
        {
          nextXHere -= stepsXHere;
          nxp -= nxa * stepsXHere;
          nyp -= nya * stepsXHere;
          nzp -= nza * stepsXHere;
          stepsXHere = 1;
          nextXHere += stepsXHere;
          nxp += nxa * stepsXHere;
          nyp += nya * stepsXHere;
          nzp += nza * stepsXHere;
          SKY_PLANE
        }
        k = 0;
        kAdd = 0x10000/stepsXHere;
      }
      if (*zBuffer >= _sky_z_value && ny>0.0) {
        int tx_0 = (((nextTextureX-lastTextureX)*k)>>16)+lastTextureX;
        int ty_0 = (((nextTextureY-lastTextureY)*k)>>16)+lastTextureY;
        unsigned int k_0 = texture[tx_0+ty_0*textureWidth];
        int tx_1 = (((nextTextureX2-lastTextureX2)*k)>>16)+lastTextureX2;
        int ty_1 = (((nextTextureY2-lastTextureY2)*k)>>16)+lastTextureY2;
        unsigned int k_1 = texture[tx_1+ty_1*textureWidth];
        unsigned int a_0 = (k_0 >> 24) & 255;
        unsigned int a_1 = (k_1 >> 24) & 255;
        if (ny<0.1) {
          float fa = ny/0.1;
          a_0*=fa;
          a_1*=fa;
        }
        unsigned int rb = (*frameBuffer & 0x00ff00ff);
        rb += ((((k_0 & 0x00ff00ff)-rb)*a_0)>>8);
        rb &= 0x00ff00ff;
        rb += ((((k_1 & 0x00ff00ff)-rb)*a_1)>>8) & 0x00ff00ff; 
        unsigned int g = (*frameBuffer & 0x0000ff00);
        g += ((((k_0 & 0x0000ff00)-g)*a_0)>>8);
        g &= 0x0000ff00;
        g += ((((k_1 & 0x0000ff00)-g)*a_1)>>8) & 0x0000ff00;
        *frameBuffer = rb|g;
      }
      k += kAdd;
      frameBuffer++;
      zBuffer++;
    }
    _0y_nxp += _0y_nxa;
    _0y_nyp += _0y_nya;
    _0y_nzp += _0y_nza;
    _1y_nxp += _1y_nxa;
    _1y_nyp += _1y_nya;
    _1y_nzp += _1y_nza;
  }
}

// GAUSSIANBLOOM FILTER
float gaussianTap5Weight0 = 0.054f;
float gaussianTap5Weight1 = 0.244f;
float gaussianTap5Weight2 = 0.403f;

#define BLOOMKERNEL\
      r=g=b=0;\
      v = weight0;\
      r+=(rgba0&255)*v;\
      g+=((rgba0>>8)&255)*v;\
      b+=((rgba0>>16)&255)*v;\
      v = weight1;\
      r+=(rgba1&255)*v;\
      g+=((rgba1>>8)&255)*v;\
      b+=((rgba1>>16)&255)*v;\
      v = weight2;\
      r+=(rgba2&255)*v;\
      g+=((rgba2>>8)&255)*v;\
      b+=((rgba2>>16)&255)*v;\
      v = weight1;\
      r+=(rgba3&255)*v;\
      g+=((rgba3>>8)&255)*v;\
      b+=((rgba3>>16)&255)*v;\
      v = weight0;\
      r+=(rgba4&255)*v;\
      g+=((rgba4>>8)&255)*v;\
      b+=((rgba4>>16)&255)*v;\
      r/=weights;\
      g/=weights;\
      b/=weights;\

void bloomX(unsigned int *buffer, unsigned int width, unsigned int height, int tap0, int tap1) {
  int weight0 =  gaussianTap5Weight0 * 0x100;
  int weight1 =  gaussianTap5Weight1 * 0x100;
  int weight2 =  gaussianTap5Weight2 * 0x100; // center
  int weights = (weight0+weight1+weight2+weight1+weight0);
  unsigned int *newBuffer = new unsigned int[width*height];
  for (int y = 0; y < height; y++) {
    unsigned int *s = &buffer[y*width];
    unsigned int *d = &newBuffer[y*width];
    int r,g,b,v;
    for (int x = 0; x < width; x++) {
      int t0x = x - tap0; if (t0x < 0) t0x = 0;
      int t1x = x - tap1; if (t1x < 0) t1x = 0;
      int t3x = x + tap1; if (t3x > width-1) t3x = width-1;
      int t4x = x + tap0; if (t4x > width-1) t4x = width-1;
      unsigned int rgba0 = s[t0x];
      unsigned int rgba1 = s[t1x];
      unsigned int rgba2 = s[x];
      unsigned int rgba3 = s[t3x];
      unsigned int rgba4 = s[t4x];
      BLOOMKERNEL
      *d++=r|(g<<8)|(b<<16)|(rgba2&0xff000000);
    }
  }
  memcpy(buffer,newBuffer,width*height*sizeof(unsigned int));
  delete[] newBuffer;
}

void bloomY(unsigned int *buffer, unsigned int width, unsigned int height, int tap0, int tap1) {
  int weight0 =  gaussianTap5Weight0 * 0x100;
  int weight1 =  gaussianTap5Weight1 * 0x100;
  int weight2 =  gaussianTap5Weight2 * 0x100; // center
  int weights = (weight0+weight1+weight2+weight1+weight0);
  unsigned int *newBuffer = new unsigned int[width*height];
  for (int x = 0; x < width; x++) {
    unsigned int *s = &buffer[x];
    unsigned int *d = &newBuffer[x];
    int r,g,b,v;
    for (int y = 0; y < height; y++) {
      int t0y = y - tap0; if (t0y < 0) t0y = 0;
      int t1y = y - tap1; if (t1y < 0) t1y = 0;
      int t3y = y + tap1; if (t3y > height-1) t3y = height-1;
      int t4y = y + tap0; if (t4y > height-1) t4y = height-1;
      unsigned int rgba0 = s[t0y*width];
      unsigned int rgba1 = s[t1y*width];
      unsigned int rgba2 = s[y*width];
      unsigned int rgba3 = s[t3y*width];
      unsigned int rgba4 = s[t4y*width];
      BLOOMKERNEL
      *d=r|(g<<8)|(b<<16)|(rgba2&0xff000000);
      d+=width;
    }
  }
  memcpy(buffer,newBuffer,width*height*sizeof(unsigned int));
  delete[] newBuffer;
}


void bloom(unsigned int *frameBuffer, unsigned int width, unsigned int height, int passes, float threshold, float gain) {
  unsigned int *bloomBuffer = new unsigned int[width*height];
  int iThreshold = threshold*256.f;
  int iGain = gain*256.f;
  {for (int i = 0; i < width*height; i++) {
    unsigned int rgba = frameBuffer[i];
    int r = rgba & 255;
    int g = (rgba>>8) & 255;
    int b = (rgba>>16) & 255;
    int a = (rgba>>24) & 255;
    r = (r * a)>>8;
    g = (g * a)>>8;
    b = (b * a)>>8;
    r -= iThreshold;
    g -= iThreshold;
    b -= iThreshold;
    r = (r * iGain)>>8;
    g = (g * iGain)>>8;
    b = (b * iGain)>>8;
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    bloomBuffer[i] = r|(g<<8)|(b<<16)|(rgba&0xff000000);
  }}
  {for (int i = passes-1; i >= 0; i--) {
    int kernelSize = pow(2,i);
    bloomX(bloomBuffer,width,height,kernelSize*2,kernelSize*1);
  }}
  {for (int i = passes-1; i >= 0; i--) {
    int kernelSize = pow(2,i);
    bloomY(bloomBuffer,width,height,kernelSize*2,kernelSize*1);
  }}
  {for (int i = 0; i < width*height; i++) {
    unsigned int rgba0 = bloomBuffer[i];
    unsigned int rgba1 = frameBuffer[i];
    int r = rgba0 & 255;
    int g = (rgba0>>8) & 255;
    int b = (rgba0>>16) & 255;
    r = (r * iGain)>>8;
    g = (g * iGain)>>8;
    b = (b * iGain)>>8;
    r += rgba1 & 255;
    g += (rgba1>>8) & 255;
    b += (rgba1>>16) & 255;
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    frameBuffer[i] = r|(g<<8)|(b<<16)|(rgba1&0xff000000);
  }}
  delete[] bloomBuffer;
}
