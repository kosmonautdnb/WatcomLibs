#include "sprite.hpp"
#include "gl.h"
#include "vector.hpp"
#include "matrix.hpp"
#include <math.h>

double spritePixelCenterX = 0.5;  // currently is different to glSpritePixelCenterX
double spritePixelCenterY = 0.5;  // currently is different to glSpritePixelCenterY
double spriteTexelCenterX = 0.0; // currently is different to glSpriteTexelCenterX
double spriteTexelCenterY = 0.0; // currently is different to glSpriteTexelCenterY

BallHeightMap *genBallHeightMap(const int width, const int height) {
  BallHeightMap *r = new BallHeightMap(width,height);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      double fx = (double)x * 2 / (width-1) - 1;
      double fy = (double)y * 2 / (height-1) - 1;
      double d = sqrt(fx*fx+fy*fy);
      if (d>=1.f) 
        d = 0.f;
      else
        d = cos(asin(d)); 
      r->data[x+y*width] = d;
    }
  }
  return r;
}

void drawBall(const class Vector &ballPos, const double ballSize, const BallHeightMap *ballHeightMap, const unsigned int ballColor) {
  Matrix modelMatrix;
  Matrix projectionMatrix;
  glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix.m);   
  glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix.m);   

  Vector p1 = transform(ballPos,modelMatrix);
  Vector p2 = transform(p1,projectionMatrix);
  if (p2.z < -p2.w || p2.z > p2.w || p2.w == 0) {
    return;
  }

  double zoomX; 
  double zoomY;
  glGetDoublev(GL_ZOOM_X,&zoomX);
  glGetDoublev(GL_ZOOM_Y,&zoomY);

  double px = projectionMatrix.m[0+0*4]*(ballSize*0.5);
  double py = projectionMatrix.m[1+1*4]*(ballSize*0.5);
  Vector pzv = p1+Vector(0,0,ballSize*0.5);
  double pzz = projectionMatrix.m[2+0*4]*pzv.x+projectionMatrix.m[2+1*4]*pzv.y+projectionMatrix.m[2+2*4]*pzv.z+projectionMatrix.m[2+3*4]*pzv.w;
  double pzw = projectionMatrix.m[3+0*4]*pzv.x+projectionMatrix.m[3+1*4]*pzv.y+projectionMatrix.m[3+2*4]*pzv.z+projectionMatrix.m[3+3*4]*pzv.w;

  if (pzz < -pzw || pzz > pzw || pzw == 0) {
    return;
  }

  double xp = p2.x/p2.w;
  double yp = p2.y/p2.w;

  int viewport[4]; glGetIntegerv(GL_VIEWPORT, viewport); viewport[1]=glFrameBufferHeight-viewport[1]-viewport[3];
  xp = (xp*0.5*zoomX+0.5)*(double)viewport[2]+(double)viewport[0];
  yp = (-yp*0.5*zoomY+0.5)*(double)viewport[3]+(double)viewport[1];

  double sx = (fabs(px)*0.5/p2.w)*(double)viewport[2]*zoomX;
  double sy = (fabs(py)*0.5/p2.w)*(double)viewport[3]*zoomY;

  double txa = -spriteTexelCenterX * sx * 2.0 / ballHeightMap->width; // inverse texel offset, may it work??
  double tya = -spriteTexelCenterY * sy * 2.0 / ballHeightMap->height;

  double fx0 = xp - sx+spritePixelCenterX+txa;
  double fy0 = yp - sy+spritePixelCenterY+tya;
  double fx1 = xp + sx+spritePixelCenterX+txa;
  double fy1 = yp + sy+spritePixelCenterY+tya;

  int ix0 = (int)floor(fx0);  if (ix0>=glFrameBufferWidth||ix0>=viewport[0]+viewport[2]) return;
  int iy0 = (int)floor(fy0);  if (iy0>=glFrameBufferHeight||iy0>=viewport[1]+viewport[3]) return;
  int ix1 = (int)floor(fx1);   if (ix1<=0||ix1<=viewport[0]) return;
  int iy1 = (int)floor(fy1);   if (iy1<=0||iy1<=viewport[1]) return;

  int tx0 = 0;
  int ty0 = 0;
  int tx1 = ballHeightMap->width;
  int ty1 = ballHeightMap->height;

  int txadd = (int)((double)(tx1-tx0)*4096.0/(fx1-fx0));
  int tyadd = (int)((double)(ty1-ty0)*4096.0/(fy1-fy0));

  tx0 = txadd-1-(int)floor((fx0-(double)ix0)*txadd);
  ty0 = tyadd-1-(int)floor((fy0-(double)iy0)*tyadd);

  if (ix0<viewport[0]) {tx0+=(viewport[0]-ix0)*txadd;ix0 = viewport[0];}
  if (iy0<viewport[1]) {ty0+=(viewport[1]-iy0)*tyadd;iy0 = viewport[1];}
  if (ix1>viewport[0]+viewport[2]) {ix1 = viewport[0]+viewport[2];}
  if (iy1>viewport[1]+viewport[3]) {iy1 = viewport[1]+viewport[3];}
  if (ix0<0) {tx0+=(0-ix0)*txadd;ix0 = 0;}
  if (iy0<0) {ty0+=(0-iy0)*tyadd;iy0 = 0;}
  if (ix1>glFrameBufferWidth) {ix1 = glFrameBufferWidth;}
  if (iy1>glFrameBufferHeight) {iy1 = glFrameBufferHeight;}
  if (glIsEnabled(GL_SCISSOR_TEST)) {
    int box[4];
    glGetIntegerv(GL_SCISSOR_BOX, box);
    box[1] = glFrameBufferHeight-box[1];
    if (ix0<box[0]) {tx0+=(box[0]-ix0)*txadd; ix0 = box[0];}
    if (iy0<box[1]) {ty0+=(box[1]-iy0)*tyadd; iy0 = box[1];}
    if (ix1>=box[0]+box[2]) { ix1 = box[0] + box[2];}
    if (iy1>=box[1]+box[3]) { iy1 = box[1] + box[3];}
  }
  if (ix0>=ix1||iy0>=iy1) return;

  double zNearFar[2]; glGetDoublev(GL_DEPTH_RANGE, zNearFar);
  double zp0 = p2.z/p2.w;
  double zp1 = pzz/pzw;
  zp0 = (zp0*0.5+0.5)*(zNearFar[1]-zNearFar[0])+zNearFar[0];
  zp1 = (zp1*0.5+0.5)*(zNearFar[1]-zNearFar[0])+zNearFar[0];
  const float zpa = zp1-zp0;
  float *data = ballHeightMap->data;
 
  {  
    unsigned int *destP0 = &glFrameBuffer[iy0 * glFrameBufferWidth+ix0];
    float *destZ0 = &glDepthBuffer[iy0 * glFrameBufferWidth+ix0];
    unsigned int ty = ty0;
    for (int y = iy0; y < iy1; y++) {
      unsigned int *destP = destP0;
      float *destZ = destZ0;
      float *data2 = &data[(ty>>12)*ballHeightMap->width];
      unsigned int tx = tx0;
      for (int i = ix1-ix0; i>0; i--) {
         //if ((tx>>12)*(ty>>12)>=ballHeightMap->width*ballHeightMap->height) {glDone;exit(0);}
         const float depth = data2[tx>>12];
         if (depth != 0.f) {
            float zp = zp0+zpa*depth;
            if (zp<=*destZ) {
              *destZ = zp;
              *destP = ballColor;
            }
         }
         tx += txadd;
         destZ++;
         destP++;
      }
      destP0 += glFrameBufferWidth;
      destZ0 += glFrameBufferWidth;
      ty += tyadd;
    }
  }
}


void drawSprite(const class Vector &spritePos, const double spriteWidth, const double spriteHeight, const unsigned int glTexture, const unsigned int spriteColor, unsigned int spriteFlags) {
  Matrix modelMatrix;
  Matrix projectionMatrix;
  glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix.m);   
  glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix.m);   

  Vector p1 = transform(spritePos,modelMatrix);
  Vector p2 = transform(p1,projectionMatrix);
  if (p2.z < -p2.w || p2.z > p2.w || p2.w == 0) {
    return;
  }
  double zoomX; 
  double zoomY;
  glGetDoublev(GL_ZOOM_X,&zoomX);
  glGetDoublev(GL_ZOOM_Y,&zoomY);

  double xp = p2.x/p2.w;
  double yp = p2.y/p2.w;

  int viewport[4]; glGetIntegerv(GL_VIEWPORT, viewport); viewport[1]=glFrameBufferHeight-viewport[1]-viewport[3];

  xp = (xp*0.5*zoomX+0.5)*(double)viewport[2]+(double)viewport[0];
  yp = (-yp*0.5*zoomY+0.5)*(double)viewport[3]+(double)viewport[1];

  double px = projectionMatrix.m[0+0*4]*(spriteWidth*0.5);
  double py = projectionMatrix.m[1+1*4]*(spriteHeight*0.5);

  double sx = (fabs(px)*0.5/p2.w)*(double)viewport[2]*zoomX;
  double sy = (fabs(py)*0.5/p2.w)*(double)viewport[3]*zoomY;

  if (spriteFlags & SPRITEFLAG_BYSCREENSIZE) {
    sx = spriteWidth * 0.5 * zoomX;
    sy = spriteHeight * 0.5 * zoomY;
  }

  int texWidth = glGetTextureWidth(glTexture);
  int texHeight = glGetTextureHeight(glTexture);
  
  double txa = -spriteTexelCenterX * sx * 2.0 / texWidth; // inverse texel offset, may it work??
  double tya = -spriteTexelCenterY * sy * 2.0 / texHeight;
  
  double fx0 = xp - sx+spritePixelCenterX+txa;
  double fy0 = yp - sy+spritePixelCenterY+tya;
  double fx1 = xp + sx+spritePixelCenterX+txa;
  double fy1 = yp + sy+spritePixelCenterY+tya;

  int ix0 = (int)floor(fx0);  if (ix0>=glFrameBufferWidth||ix0>=viewport[0]+viewport[2]) return;
  int iy0 = (int)floor(fy0);  if (iy0>=glFrameBufferHeight||iy0>=viewport[1]+viewport[3]) return;
  int ix1 = (int)floor(fx1);   if (ix1<=0||ix1<=viewport[0]) return;
  int iy1 = (int)floor(fy1);   if (iy1<=0||iy1<=viewport[1]) return;

  int tx0 = 0;
  int ty0 = 0;
  int tx1 = texWidth;
  int ty1 = texHeight;

  int txadd = (int)((double)(tx1-tx0)*4096.0/(fx1-fx0));
  int tyadd = (int)((double)(ty1-ty0)*4096.0/(fy1-fy0));

  tx0 = txadd-1-(int)floor((fx0-(double)ix0)*txadd);
  ty0 = tyadd-1-(int)floor((fy0-(double)iy0)*tyadd);

  if (spriteFlags & SPRITEFLAG_NOT_SUBPIXEL) {
    tx0 = 0;
    ty0 = 0;
    txadd = (int)((double)(tx1-tx0)*4096.0/(ix1-ix0));
    tyadd = (int)((double)(ty1-ty0)*4096.0/(iy1-iy0));
  }

  if (ix0<viewport[0]) {tx0+=(viewport[0]-ix0)*txadd;ix0 = viewport[0];}
  if (iy0<viewport[1]) {ty0+=(viewport[1]-iy0)*tyadd;iy0 = viewport[1];}
  if (ix1>viewport[0]+viewport[2]) {ix1 = viewport[0]+viewport[2];}
  if (iy1>viewport[1]+viewport[3]) {iy1 = viewport[1]+viewport[3];}
  if (ix0<0) {tx0+=(0-ix0)*txadd;ix0 = 0;}
  if (iy0<0) {ty0+=(0-iy0)*tyadd;iy0 = 0;}
  if (ix1>glFrameBufferWidth) {ix1 = glFrameBufferWidth;}
  if (iy1>glFrameBufferHeight) {iy1 = glFrameBufferHeight;}
  if (glIsEnabled(GL_SCISSOR_TEST)) {
    int box[4];
    glGetIntegerv(GL_SCISSOR_BOX, box);
    box[1] = glFrameBufferHeight-box[1];
    if (ix0<box[0]) {tx0+=(box[0]-ix0)*txadd; ix0 = box[0];}
    if (iy0<box[1]) {ty0+=(box[1]-iy0)*tyadd; iy0 = box[1];}
    if (ix1>=box[0]+box[2]) { ix1 = box[0] + box[2];}
    if (iy1>=box[1]+box[3]) { iy1 = box[1] + box[3];}
  }
  if (ix0>=ix1||iy0>=iy1) return;

  double zNearFar[2]; glGetDoublev(GL_DEPTH_RANGE, zNearFar);
  double zp0 = p2.z/p2.w;
  zp0 = (zp0*0.5+0.5)*(zNearFar[1]-zNearFar[0])+zNearFar[0];
  unsigned int *data = glGetTexturePointer(glTexture);
  const bool depthWrite = (spriteFlags & SPRITEFLAG_NODEPTHWRITE) ? false : true;
  const bool depthCompare = (spriteFlags & SPRITEFLAG_NODEPTHCOMPARE) ? false : true;
  if (!depthCompare) zp0 = -1.f; // farplane
  int sp_r = spriteColor & 255;
  int sp_g = (spriteColor>>8) & 255;
  int sp_b = (spriteColor>>16) & 255;
  int sp_a = (spriteColor >> 24) & 255;

  {  
    unsigned int *destP0 = &glFrameBuffer[iy0 * glFrameBufferWidth+ix0];
    float *destZ0 = &glDepthBuffer[iy0 * glFrameBufferWidth+ix0];
    unsigned int ty = ty0;
    for (int y = iy0; y < iy1; y++) {
      unsigned int *destP = destP0;
      float *destZ = destZ0;
      unsigned int *data2 = &data[(ty>>12)*texWidth];
      unsigned int tx = tx0;
      if (spriteFlags & SPRITEFLAG_ADDITIVE) {
        for (int i = ix1-ix0; i>0; i--) {
          const unsigned int rgba = data2[tx>>12];
          int a = (((rgba>>24)&255)*sp_a)>>8;
          if (zp0<=*destZ) {
            if (depthWrite) *destZ = zp0;
            int rh = *destP & 255;
            int gh = (*destP>>8) & 255;
            int bh = (*destP>>16) & 255;
            const int rt = rgba & 255;
            const int gt = (rgba>>8) & 255;
            const int bt = (rgba>>16) & 255;
            rh += (rt*sp_r)>>8;
            gh += (gt*sp_g)>>8;
            bh += (bt*sp_b)>>8;
            if (rh > 255) rh = 255;
            if (gh > 255) gh = 255;
            if (bh > 255) bh = 255;
            *destP = rh|(gh<<8)|(bh<<16)|0xff000000;
          }
          tx += txadd;
          destZ++;
          destP++;
        }
      } 
      else { // SPRITEFLAG_ADDITIVE else
        if (spriteFlags & SPRITEFLAG_PREMULTIPLIED_ALPHA) {
          for (int i = ix1-ix0; i>0; i--) {
            const unsigned int rgba = data2[tx>>12];
            int a = (((rgba>>24)&255)*sp_a)>>8;
            if (zp0<=*destZ) {
              if (depthWrite) *destZ = zp0;
              int rh = *destP & 255;
              int gh = (*destP>>8) & 255;
              int bh = (*destP>>16) & 255;
              const int rt = rgba & 255;
              const int gt = (rgba>>8) & 255;
              const int bt = (rgba>>16) & 255;
              a = 255-a;
              rh = (rh*a+rt*sp_r)>>8;
              gh = (gh*a+gt*sp_g)>>8;
              bh = (bh*a+bt*sp_b)>>8;
              if (rh > 255) rh = 255;
              if (gh > 255) gh = 255;
              if (bh > 255) bh = 255;
              *destP = rh|(gh<<8)|(bh<<16)|0xff000000;
            }
            tx += txadd;
            destZ++;
            destP++;
          }
        } else { // SPRITEFLAG_PREMULTIPLIED_ALPHA else
          for (int i = ix1-ix0; i>0; i--) {
            const unsigned int rgba = data2[tx>>12];
            int a = (((rgba>>24)&255)*sp_a)>>8;
            if (a != 0) {
              if (zp0<=*destZ) {
                if (depthWrite) *destZ = zp0;
                int rh = *destP & 255;
                int gh = (*destP>>8) & 255;
                int bh = (*destP>>16) & 255;
                const int rt = rgba & 255;
                const int gt = (rgba>>8) & 255;
                const int bt = (rgba>>16) & 255;
                rh += ((((rt*sp_r)>>8) - rh)*a)>>8;
                gh += ((((gt*sp_g)>>8) - gh)*a)>>8;
                bh += ((((bt*sp_b)>>8) - bh)*a)>>8;
                *destP = rh|(gh<<8)|(bh<<16)|0xff000000;
              }
            }
            tx += txadd;
            destZ++;
            destP++;
          }
        }
      }
      destP0 += glFrameBufferWidth;
      destZ0 += glFrameBufferWidth;
      ty += tyadd;
    }
  }
}
