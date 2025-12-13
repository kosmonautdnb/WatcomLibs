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
