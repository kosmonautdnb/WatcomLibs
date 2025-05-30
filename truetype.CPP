#include "GL.H"
#include "object.hpp"
#include "string.hpp"
#include "truetype.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stbttf.hpp"

#define GLYPH_START 32
#define GLYPH_COUNT 96

stbtt_bakedchar cdata[GLYPH_COUNT]; // ASCII 32..126 is 95 glyphs
GLuint ftex = 0;

unsigned int stb_initfont(const char *ttf_file_name, float fontHeight)
{
  int width = 512;
  int height = 512;
  int ttf_buffer_size = 1<<20;
  FILE *in = fopen(ttf_file_name, "rb");
  if (in == NULL) return 0;
  unsigned char *temp_bitmap = new unsigned char[width*height];
  unsigned char *ttf_buffer = new unsigned char[ttf_buffer_size];
  fread(ttf_buffer,1,ttf_buffer_size,in);
  fclose(in);
  stbtt_BakeFontBitmap(ttf_buffer,0,fontHeight,temp_bitmap,width,height,GLYPH_START,GLYPH_COUNT,cdata); // no guarantee this fits!
  delete[] ttf_buffer;
  glGenTextures(1, &ftex);
  glBindTexture(GL_TEXTURE_2D, ftex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
  delete[] temp_bitmap;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  return ftex;
}

void stb_print(float x, float y, const char *text, uint32_t color, float anchorX, float anchorY)
{
  // assume orthographic projection with units = screen pixels, origin at top left
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, ftex);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER,0.1);
  
  glColor4ubv((GLubyte*)&color);
  glVertex4f(0,0,0,1.f);
  glDepthMask(GL_FALSE);
  
  float minX,minY,maxX,maxY;
  const char *text2 = text;
  float x2 = x;
  float y2 = y;
  bool first = true;
  while (*text2) {
    if (*text2 >= 32 && *text2 < 128) {
      stbtt_aligned_quad q;
      stbtt_GetBakedQuad(cdata, 512,512, *text2-32, &x2,&y2,&q,1);//1=opengl & d3d10+,0=d3d9
      if (q.x0<minX||first) minX = q.x0;
      if (q.y0<minY||first) minY = q.y0;
      if (q.x1>maxX||first) maxX = q.x1;
      if (q.y1>maxY||first) maxY = q.y1;
      first = false;
    }
    ++text2;
  }

  float zoomX = 1.f;
  float zoomY = 1.f;
  glGetFloatv(GL_ZOOM_X,&zoomX);
  glGetFloatv(GL_ZOOM_Y,&zoomY);

  x -= (maxX-minX)/zoomX*anchorX;
  y -= (maxY-minY)/zoomY*anchorY;

  glBegin(GL_QUADS);
  while (*text) {
    if (*text >= 32 && *text < 128) {
      stbtt_aligned_quad q;
      stbtt_GetBakedQuad(cdata, 512,512, *text-32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
      glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y1);
      glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y1);
      glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y0);
      glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y0);
    }
    ++text;
  }
  glEnd();

  glDepthMask(GL_TRUE);
  glDisable(GL_ALPHA_TEST);
}

void glDrawTextTTF(bool clip, float xp, float yp, float zp, const float scale, const char *text, uint32_t color, float anchorX, float anchorY) {
  // on ortho use zp=-1 for nearplane
  double model[16];
  double projection[16];
  int view[4];
  glGetDoublev(GL_MODELVIEW_MATRIX,model);
  glGetDoublev(GL_PROJECTION_MATRIX,projection);
  glGetIntegerv(GL_VIEWPORT,view);
  double sx1,sy1,sz1;  
  gluProjectx(xp,yp,zp,model,projection,view,&sx1,&sy1,&sz1);
  //if (abs(sz1) >= 1.0) return; // i don't know what glu makes here, >1 seems to be the far plane? or the near plane?   

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(view[0],view[0]+view[2],view[1]+view[3],view[1],-1,1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  anchorY -= 1.0; // ttf uses lower line as baseline instead of top  
  glScalef(scale,scale,1);
  stb_print(sx1/scale,(view[1]+view[3]-sy1)/scale,text,color,anchorX, anchorY);

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

