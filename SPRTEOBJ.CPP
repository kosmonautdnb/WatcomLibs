#include "sprteobj.hpp"
#include "gl.h"
#include "vector.hpp"
#include "matrix.hpp"

void createSpriteObjectFrameBuffer(SpriteObject *sp, int w, int h) {
  sp->width = w;
  sp->height = h;
  glGenTextures(1,&sp->frameBufferColor);
  glBindTexture(GL_TEXTURE_2D, sp->frameBufferColor);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, w, h, 0, GL_RGBA, GL_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glGenTextures(1,&sp->frameBufferDepth);
  glBindTexture(GL_TEXTURE_2D, sp->frameBufferDepth);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D,0);
  
  glGenFramebuffers(1,&sp->frameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, sp->frameBuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sp->frameBufferColor, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sp->frameBufferDepth, 0);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

static int spViewport[4];
static int spScissor[4];

void startSpriteObjectPainting(SpriteObject *sp, const Vector *boundingMin, const Vector *boundingMax) {
  glGetIntegerv(GL_VIEWPORT,spViewport);
  glGetIntegerv(GL_SCISSOR_BOX,spScissor);
  glBindFramebuffer(GL_FRAMEBUFFER, sp->frameBuffer);
  glScissor(0,0,sp->width,sp->height);
  glViewport(0,0,sp->width,sp->height);
  glClearDepth(1.f);
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  
  Matrix mv;
  glGetDoublev(GL_MODELVIEW_MATRIX, mv.m);
  glLoadIdentity();
  // actually all this "Matrix Building" here is not really needed
  Vector _x = normalize(mv.getRow(0));
  Vector _y = normalize(mv.getRow(1));
  Vector _z = normalize(mv.getRow(2));
  /*_x.y = 0;
  _y.y = 0;
  _z.y = 0;*/
  _x.w = 0;
  _y.w = 0;
  _z.w = 0;
  _y = normalize(cross(_z,_x));
  _x = normalize(cross(_y,_z));
  _z = normalize(cross(_x,_y));
  _x = normalize(cross(_y,_z));
  _z = normalize(cross(_x,_y));
  _y = normalize(cross(_z,_x));
  _x.w = 0;
  _y.w = 0;
  _z.w = 0;
  mv.setRow(0,_x);
  mv.setRow(1,_y);
  mv.setRow(2,_z);
  mv.setRow(3,Vector());
  glLoadMatrixd(mv.m);
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-1,1,-1,1,-10000.0,10000.0);
  
  Matrix modelView,projection;
  glGetDoublev(GL_MODELVIEW_MATRIX,modelView.m);
  glGetDoublev(GL_PROJECTION_MATRIX,projection.m);
  Matrix modelViewProjection = projection * modelView;
  Vector mn,mx;
  for (int j = 0; j < 8; j++) {
    Vector p2;
    p2.x = (j & 1) ? boundingMin->x : boundingMax->x;
    p2.y = (j & 2) ? boundingMin->y : boundingMax->y;
    p2.z = (j & 4) ? boundingMin->z : boundingMax->z;
    p2 = modelViewProjection * p2;
    if (p2.w != 0.0) p2 /= p2.w;
    if (j == 0 || p2.x<mn.x) mn.x = p2.x;
    if (j == 0 || p2.y<mn.y) mn.y = p2.y;
    if (j == 0 || p2.x>mx.x) mx.x = p2.x;
    if (j == 0 || p2.y>mx.y) mx.y = p2.y;
  }
  glLoadIdentity();
  glOrtho(mn.x,mx.x,mn.y,mx.y,-10000.0,10000.0);
  sp->minx = mn.x;
  sp->miny = mn.y;
  sp->maxx = mx.x;
  sp->maxy = mx.y;
}

void finishSpriteObjectPainting() {
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  glViewport(spViewport[0],spViewport[1],spViewport[2],spViewport[3]);
  glScissor(spScissor[0],spScissor[1],spScissor[2],spScissor[3]);

}

void blitSpriteObjectSprite(const SpriteObject *sp, const class Vector *pos, double scale, unsigned int color) {
  Matrix k;
  glGetDoublev(GL_MODELVIEW_MATRIX, k.m);

  const float z2 = pos->x * k.m[0*4+2] + pos->y * k.m[1*4+2] + pos->z * k.m[2*4+2] + k.m[3*4+2];
  if (z2 > 0) return; // may conflict with some GL_PROJECTION matrices

  k = k.matrix3x3();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(pos->x, pos->y, pos->z);
  glMultMatrixd(transpose(k).m);
  const Vector mn = Vector(sp->minx,sp->miny)*scale;
  const Vector mx = Vector(sp->maxx,sp->maxy)*scale;
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER,0.5);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sp->frameBufferColor);
  glVertex4f(0,0,0,1);
  glBegin(GL_QUADS);
  glColor4ubv((GLubyte*)&color);
  glTexCoord2f(1,0); glVertex3f(mx.x,mx.y,0);
  glTexCoord2f(0,0); glVertex3f(mn.x,mx.y,0);
  glTexCoord2f(0,1); glVertex3f(mn.x,mn.y,0);
  glTexCoord2f(1,1); glVertex3f(mx.x,mn.y,0);
  glEnd();
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

#include <math.h>
static double spritePixelCenterX = 0.5;
static double spritePixelCenterY = 0.5;
static double spriteTexelCenterX = 0.0;
static double spriteTexelCenterY = 0.0;
#define SPRITESHIFT 14

// alphatest "is" gl_greater here
void blitSpriteObjectSpriteNoTexture(const SpriteObject *sp, const class Vector *pos, double scale, unsigned int colorMul, unsigned int colorAdd) {
  Matrix modelMatrix;
  Matrix projectionMatrix;
  glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix.m);   
  glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix.m);   
  double ref; glGetDoublev(GL_ALPHA_TEST_REF, &ref);

  const Vector mn = Vector(sp->minx,sp->miny)*scale;
  const Vector mx = Vector(sp->maxx,sp->maxy)*scale;

  Vector p1 = transform(*pos,modelMatrix);
  p1.x += (mn.x + mx.x) * 0.5;
  p1.y += (mn.y + mx.y) * 0.5;
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

  const double spriteWidth = mx.x - mn.x;
  const double spriteHeight = mx.y - mn.y;

  double px = projectionMatrix.m[0+0*4]*(spriteWidth*0.5);
  double py = projectionMatrix.m[1+1*4]*(spriteHeight*0.5);

  double sx = (fabs(px)*0.5/p2.w)*(double)viewport[2]*zoomX;
  double sy = (fabs(py)*0.5/p2.w)*(double)viewport[3]*zoomY;

  int texWidth = glGetTextureWidth(sp->frameBufferColor);
  int texHeight = glGetTextureHeight(sp->frameBufferColor);
  
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

  int txadd = (int)((double)(tx1-tx0)*(1<<SPRITESHIFT)/(fx1-fx0));
  int tyadd = (int)((double)(ty1-ty0)*(1<<SPRITESHIFT)/(fy1-fy0));

  tx0 += txadd-1-(int)floor((fx0-(double)ix0)*txadd);
  ty0 += tyadd-1-(int)floor((fy0-(double)iy0)*tyadd);

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
    box[1] = glFrameBufferHeight-box[1]-box[3];
    if (ix0<box[0]) {tx0+=(box[0]-ix0)*txadd; ix0 = box[0];}
    if (iy0<box[1]) {ty0+=(box[1]-iy0)*tyadd; iy0 = box[1];}
    if (ix1>=box[0]+box[2]) { ix1 = box[0] + box[2];}
    if (iy1>=box[1]+box[3]) { iy1 = box[1]+box[3];}
  }
  if (ix0>=ix1||iy0>=iy1) return;

  double zNearFar[2]; glGetDoublev(GL_DEPTH_RANGE, zNearFar);
  double zp0 = p2.z/p2.w;
  zp0 = (zp0*0.5+0.5)*(zNearFar[1]-zNearFar[0])+zNearFar[0];
  unsigned int *data = glGetTexturePointer(sp->frameBufferColor);
  int sp_r = colorMul & 255;
  int sp_g = (colorMul >> 8) & 255;
  int sp_b = (colorMul >> 16) & 255;
  int sp_a = (colorMul >> 24) & 255;
  int sp2_r = colorAdd & 255;
  int sp2_g = (colorAdd >> 8) & 255;
  int sp2_b = (colorAdd >> 16) & 255;
  int sp2_a = (colorAdd >> 24) & 255;
  int alphaRef = ref * 255; // assuming greater by default here

  unsigned int *destP0 = &glFrameBuffer[iy0 * glFrameBufferWidth+ix0];
  float *destZ0 = &glDepthBuffer[iy0 * glFrameBufferWidth+ix0];
  unsigned int ty = ty0;
  for (int y = iy0; y < iy1; y++) {
    unsigned int *destP = destP0;
    float *destZ = destZ0;
    unsigned int *data2 = &data[(ty>>SPRITESHIFT)*texWidth];
    unsigned int tx = tx0;
    for (int i = ix1-ix0; i>0; i--) {
      const unsigned int rgba = data2[tx>>SPRITESHIFT];
      const int a = (rgba>>24)&255;
      if (a > alphaRef) {
        if (zp0<=*destZ) {
          *destZ = zp0;
          int rh = (((rgba & 255) * sp_r)>>8) + sp2_r;
          int gh = ((((rgba>>8) & 255) * sp_g)>>8) + sp2_g;
          int bh = ((((rgba>>16) & 255) * sp_b)>>8) + sp2_b;
          if (rh > 255) rh = 255;
          if (gh > 255) gh = 255;
          if (bh > 255) bh = 255;
          *destP = rh|(gh<<8)|(bh<<16)|(colorMul&0xff000000);
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
