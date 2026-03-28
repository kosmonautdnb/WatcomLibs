#ifndef __SUMARETB_HPP__
#define __SUMARETB_HPP__

static void calcSummedAreaTable(float *table, const float *data, int width, int height) {
  float *dest = table;
  const float *src = data;
  float lastX = 0;
  for (int x = 0; x < width; x++) {
    *dest = *src + lastX; lastX = *dest; dest++; src++;
  }
  for (int y = 1; y < height; y++) {
    lastX = 0;
    float *dest2 = dest-width;
    *dest = *src + lastX + *dest2; lastX = *dest; dest++; dest2++; src++;
    for (int x = 1; x < width; x++) {
      *dest = *src + lastX + *dest2 - *(dest2-1); lastX = *dest; dest++; dest2++; src++;
    }
  }
}

static inline float getSummedArea(const float *table, int width, int height, int x0,int y0, int x1,int y1) {
  if ((unsigned int)(x0|x1)>=width) {
    if (x0 >= width) x0 = width-1;
    if (x1 >= width) x1 = width-1;
    if (x0 < 0) x0 = 0;
    if (x1 < 0) x1 = 0;
  }
  if ((unsigned int)(y0|y1)>=height) {
    if (y0 >= height) y0 = height-1;
    if (y1 >= height) y1 = height-1;
    if (y0 < 0) y0 = 0;
    if (y1 < 0) y1 = 0;
  }
  float count = (float)((x1-x0)*(y1-y0));
  if (count == 0) return 0;
  return (table[x1+y1*width]-table[x0+y1*width]-table[x1+y0*width]+table[x0+y0*width])/count;
} 

#endif //__SUMARETB_HPP__
