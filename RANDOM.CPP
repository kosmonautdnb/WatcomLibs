#include "random.hpp"
#include <math.h>

float rand2df(const float x, const float y) {
  const int xi = (int)floor(x);
  const int yi = (int)floor(y);
  const float fx = x - (float)xi;
  const float fy = y - (float)yi;
  const int fxi = fx * 0x400;
  const int fyi = fy * 0x400;
  const unsigned short p00 = rand2d(xi,yi);
  const unsigned short p10 = rand2d(xi+1,yi);
  const unsigned short p11 = rand2d(xi+1,yi+1);
  const unsigned short p01 = rand2d(xi,yi+1);
  const unsigned short top = (((p10 - p00) * fxi)>>10) + p00;
  const unsigned short btm = (((p11 - p01) * fxi)>>10) + p01;
  return (float)((((btm-top)*fyi)>>10)+top)/0x10000;
}

static float rand2dfzi(const float x, const float y, const int zi) {
  const int xi = (int)floor(x);
  const int yi = (int)floor(y);
  const float fx = x - (float)xi;
  const float fy = y - (float)yi;
  const int fxi = fx * 0x400;
  const int fyi = fy * 0x400;
  const unsigned short p00 = rand3d(xi,yi,zi);
  const unsigned short p10 = rand3d(xi+1,yi,zi);
  const unsigned short p11 = rand3d(xi+1,yi+1,zi);
  const unsigned short p01 = rand3d(xi,yi+1,zi);
  const unsigned short top = (((p10 - p00) * fxi)>>10) + p00;
  const unsigned short btm = (((p11 - p01) * fxi)>>10) + p01;
  return (float)((((btm-top)*fyi)>>10)+top)/0x10000;
}

float rand3df(const float x, const float y, const float z) {
  const int zi = (int)floor(z);
  const float top = rand2dfzi(x,y,zi);
  const float bottom = rand2dfzi(x,y,zi+1);
  const float zf = z - (float)zi;
  return (bottom-top)*zf+top;
}
