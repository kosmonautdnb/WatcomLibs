#ifndef __SPRTEOBJ_HPP__
#define __SPRTEOBJ_HPP__

// a 3d billboard class

typedef struct SpriteObject {
  double minx,miny,maxx,maxy; // sprite sizes
  unsigned int width, height; // texture/framebuffer width/height
  unsigned int frameBuffer;
  unsigned int frameBufferColor;
  unsigned int frameBufferDepth;
} SpriteObject;

void createSpriteObjectFrameBuffer(SpriteObject *sp, int w, int h);
void startSpriteObjectPainting(SpriteObject *sp, const class Vector *boundingMin, const class Vector *boundingMax);
void finishSpriteObjectPainting();
void blitSpriteObjectSprite(const SpriteObject *sp, const class Vector *pos, double scale, unsigned int color);

#endif //__SPRTEOBJ_HPP__
