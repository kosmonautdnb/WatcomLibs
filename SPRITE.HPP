#ifndef __SPRITE_HPP__
#define __SPRITE_HPP__

class BallHeightMap {
public:
  BallHeightMap(const int width, const int height) : width(width), height(height) {
    data = new float[width*height];
  }
  ~BallHeightMap() {
    delete[] data;
  }
  int width, height;
  float *data;
};

BallHeightMap *genBallHeightMap(const int width, const int height);
void drawBall(const class Vector &ballPos, const double ballSize, const BallHeightMap *ballHeightMap, const unsigned int ballColor);

#define SPRITEFLAG_NONE 0
#define SPRITEFLAG_PREMULTIPLIED_ALPHA 1
#define SPRITEFLAG_NOT_SUBPIXEL 2 // only important for stretched sprites and their subpixel movement
#define SPRITEFLAG_ADDITIVE 4
#define SPRITEFLAG_NODEPTHWRITE 8
#define SPRITEFLAG_NODEPTHCOMPARE 16
#define SPRITEFLAG_BYSCREENSIZE 32 // actual screen pixel size in width/height not a "3d" size (glOrtho)
#define SPRITEFLAG_NOT_FLIP_Y 64 // for non OpenGL use cases
void drawSprite(const class Vector &spritePos, const double spriteWidth, const double spriteHeight, const unsigned int glTexture, const unsigned int spriteColor, unsigned int spriteFlags);

#endif //__SPRITE_HPP__
