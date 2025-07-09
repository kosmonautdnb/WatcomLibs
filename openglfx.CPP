#include "openglfx.hpp"
#include <string.h>
#include <math.h>

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
