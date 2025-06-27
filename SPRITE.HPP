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
void drawSprite(const class Vector &spritePos, const double spriteWidth, const double spriteHeight, const unsigned int glTexture, const unsigned int spriteColor);

#endif //__SPRITE_HPP__
