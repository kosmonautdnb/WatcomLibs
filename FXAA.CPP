#include "fxaa.hpp"
#include <string.h>
#include <math.h>

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
