#include "GL.H"
#include "object.hpp"
#include "string.hpp"
#include "truetype.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stbttf.hpp"

#define GLYPH_START 32
#define GLYPH_COUNT 96

stbtt_bakedchar cdata[MAX_FONTS][GLYPH_COUNT]; // ASCII 32..126 is 95 glyphs
GLuint ftex[MAX_FONTS] = {0};

unsigned int stb_initfont(int fontIndex, const char *ttf_file_name, float fontHeight)
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
  stbtt_BakeFontBitmap(ttf_buffer,0,fontHeight,temp_bitmap,width,height,GLYPH_START,GLYPH_COUNT,cdata[fontIndex]); // no guarantee this fits!
  delete[] ttf_buffer;
  glGenTextures(1, &ftex[fontIndex]);
  glBindTexture(GL_TEXTURE_2D, ftex[fontIndex]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
  delete[] temp_bitmap;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  return ftex[fontIndex];
}

void stb_clearfont(int fontIndex) {
  glDeleteTextures(1,&ftex[fontIndex]);
}

void stb_print(int fontIndex, float x, float y, const char *text, uint32_t color, float anchorX, float anchorY)
{
  // assume orthographic projection with units = screen pixels, origin at top left
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, ftex[fontIndex]);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER,0.1);

  glColor4ubv((GLubyte*)&color);
  glDepthMask(GL_FALSE);
  
  float minX=x,minY=y,maxX=x,maxY=y;
  const char *text2 = text;
  float x2 = x;
  float y2 = y;
  while (*text2) {
    if (*text2 >= 32 && *text2 < 128) {
      stbtt_aligned_quad q;
      stbtt_GetBakedQuad(cdata[fontIndex], 512,512, *text2-32, &x2,&y2,&q,1);//1=opengl & d3d10+,0=d3d9
      if (q.x0<minX) minX = q.x0;
      if (q.x1<minX) minX = q.x1;
      if (q.y0<minY) minY = q.y0;
      if (q.y1<minY) minY = q.y1;
      if (q.x0>maxX) maxX = q.x0;
      if (q.x1>maxX) maxX = q.x1;
      if (q.y0>maxY) maxY = q.y0;
      if (q.y1>maxY) maxY = q.y1;
    }
    if (*text2 == '\n') {
      x2 = x;
      y2 = minY;
    }
    ++text2;
  }

  x -= (maxX-minX)*anchorX;
  y -= (maxY-minY)*anchorY;
  float x3 = x;

  minY = y;
  glBegin(GL_QUADS);
  while (*text) {
    if (*text >= 32 && *text < 128) {
      stbtt_aligned_quad q;
      stbtt_GetBakedQuad(cdata[fontIndex], 512,512, *text-32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
      glTexCoord2f(q.s0,q.t1); glVertex4f(q.x0,q.y1,0,1);
      glTexCoord2f(q.s1,q.t1); glVertex4f(q.x1,q.y1,0,1);
      glTexCoord2f(q.s1,q.t0); glVertex4f(q.x1,q.y0,0,1);
      glTexCoord2f(q.s0,q.t0); glVertex4f(q.x0,q.y0,0,1);
      if (q.y0<minY) minY = q.y0;
    }
    if (*text == '\n') {
      x = x3;
      y =minY;
    }
    ++text;
  }
  glEnd();

  glDepthMask(GL_TRUE);
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}

void glDrawTextTTF(int fontIndex, float xp, float yp, float zp, const float scale, const char *text, uint32_t color, float anchorX, float anchorY) {
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
  stb_print(fontIndex, sx1/scale,sy1/scale,text,color,anchorX, anchorY);

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

