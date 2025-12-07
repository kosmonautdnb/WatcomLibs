#include <math.h>

void generateDistanceFieldBitmap(const unsigned int *src, int srcWidth, int srcHeight, unsigned int srcColor, unsigned char *dst, int dstWidth, int dstHeight, int box) {
  srcColor &= 0x00ffffff;
  const double mag = box * sqrt(2.0);
  for (int y = 0; y < dstHeight; y++) {
    const int hy = y * srcHeight / dstHeight;
    int y0 = hy - box;
    int y1 = hy + box;
    if (y0 < 0) y0 = 0;
    if (y1 >= srcHeight) y1 = srcHeight - 1;
    for (int x = 0; x < dstWidth; x++) {
      const int hx = x * srcWidth / dstWidth;
      int x0 = hx - box;
      int x1 = hx + box;
      if (x0 < 0) x0 = 0;
      if (x1 >= srcWidth) x1 = srcWidth - 1;
      const unsigned int s2 = src[hx+hy*srcWidth] & 0x00ffffff;
      double nearestD = 1.0;
      for (int ky = y0; ky < y1+1; ky++) {
        for (int kx = x0; kx < x1+1; kx++) {
          const unsigned int s = src[kx+ky*srcWidth] & 0x00ffffff;
          if ( ((s2 == srcColor) && (s != srcColor)) ||
               ((s2 != srcColor) && (s == srcColor)) ) {
            const double dx = (double)(kx - hx);
            const double dy = (double)(ky - hy);
            const double d = sqrt(dx*dx+dy*dy)/mag;
            if (d<nearestD) nearestD=d;
          }
        }
      }
      if (s2 != srcColor) nearestD *= -1;
      *dst++ = (unsigned char)((0.5+nearestD*0.5)*255.0);
    }
  }
}

/*
#include "GL.H"
#include "IMAGE.HPP"

int main(int argc, const char **argv) {

  glVesa(640,480,32);

  RGBAImage r = RGBAImage::fromFile("c:/GRAPHICS/TEXTE/lykiatxt.png");

  int scale = 5;
  int bwWidth = r.width / scale;
  int bwHeight = r.height / scale;
  double s = 1.0;
  unsigned char *bw = new unsigned char[bwWidth*bwHeight];
  generateDistanceFieldBitmap(r.data,r.width,r.height,0xffffffff,bw,bwWidth,bwHeight,scale);
                                                                                                                                                          
  unsigned int tex;
  glGenTextures(1,&tex);
  glBindTexture(GL_TEXTURE_2D,tex);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, bwWidth, bwHeight, 0, GL_ALPHA, GL_BYTE, bw);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  bool alphaTest = true;
  double alphaVal = 0.5;

  while(1) {
    int currentKey = glNextKey();
    if (currentKey == GL_VK_ESCAPE) break;
    if (currentKey == 'a') alphaTest = !alphaTest;
    if (currentKey == '+') alphaVal += 0.01;
    if (currentKey == '-') alphaVal -= 0.01;


    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    if (alphaTest) {
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GREATER,alphaVal);
    } else {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glColor4f(1,1,1,1);
    glTexCoord2f(1,0); glVertex2f(s,s);
    glTexCoord2f(0,0); glVertex2f(-s,s);
    glTexCoord2f(0,1); glVertex2f(-s,-s);
    glTexCoord2f(1,1); glVertex2f(s,-s);
    glEnd();
    glRefresh();
  }

  glDone();
  return 0;
}
*/
