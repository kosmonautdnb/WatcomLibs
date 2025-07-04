#include "fxtoon.hpp"
#include <math.h>

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

void toonShader(unsigned int *buffer, int width, int height) {
  unsigned int *dest = buffer;
  const int toToonColorNr = (100*100*255/toonColorCount);
  const int toonAdd2 = (int)floor(toonBaseLevel*100)*(100*255);
  const int toonSaturation2 = (int)floor(toonSaturation*256.0);
  const int toonLuminance2 = (int)floor(toonLuminance*100.0);
  int *lastSpanA = new int[width];
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
      } else if (toonLuminances) {
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
