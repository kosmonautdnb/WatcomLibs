#include "GLEGA.HPP"
#define WATCOMGLAPI
#include "GL.H"
#include <stdlib.h>

#ifdef __WATCOMC__
#include <i86.h>
#include <conio.h>
#endif // __WATCOMC__
#ifdef __DJGPP__
#include <dos.h>
#include <sys/nearptr.h>
#include <sys/farptr.h>
#endif // __DJGPP__

bool glIsEga = false;
bool glEgaHCR = false;

static unsigned short *glEgaHcrRemap = NULL;
static unsigned int palette[16];
static GLubyte *glEgaIndexBuffer = NULL;
static unsigned short glEgaGraphicsModeToRestore;
static float glLumis[3] = {0.2126,0.7152,0.0722};

#ifdef __WATCOMC__
#define DJGPPVGAMEMON
#define DJGPPVGAMEMOFF
#define VGARAMBASE (0xa0000)
#endif //__WATCOMC__

#ifdef __DJGPP__
#define VGARAMBASE (0xa0000+__djgpp_conventional_base)
#define DJGPPVGAMEMON if (useNearPointers) __djgpp_nearptr_enable();
#define DJGPPVGAMEMOFF if (useNearPointers) __djgpp_nearptr_disable();
#endif // __DJGPP__

#ifdef __WATCOMC__
#define INLINE static __inline
#endif // __WATCOMC__

#ifdef __DJGPP__
#define INLINE static  inline
#endif // __DJGPP__

#ifndef INLINE
#define INLINE static inline
#endif // INLINE

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


extern GLint glFrameBufferWidth0;
extern GLint glFrameBufferHeight0;
extern GLuint *glFrameBuffer0;
extern GLfloat *glDepthBuffer0;
extern GLubyte *glStencilBuffer0;

#ifdef __cplusplus
}
#endif // __cplusplus


#define EGA_COLOR_BLACK         0x00
#define EGA_COLOR_BLUE          0x01
#define EGA_COLOR_GREEN         0x02
#define EGA_COLOR_CYAN          0x03
#define EGA_COLOR_RED           0x04
#define EGA_COLOR_VIOLET        0x05
#define EGA_COLOR_BROWN         0x06
#define EGA_COLOR_GREY_LIGHT    0x07
#define EGA_COLOR_GREY_DARK     0x08
#define EGA_COLOR_BLUE_LIGHT    0x09
#define EGA_COLOR_GREEN_LIGHT   0x0a
#define EGA_COLOR_CYAN_LIGHT    0x0b
#define EGA_COLOR_RED_LIGHT     0x0c
#define EGA_COLOR_VIOLET_LIGHT  0x0d
#define EGA_COLOR_YELLOW        0x0e
#define EGA_COLOR_WHITE         0x0f

#define _EGA_COLOR_BLACK         0xff000000
#define _EGA_COLOR_BLUE          0xffaa0000
#define _EGA_COLOR_GREEN         0xff00aa00
#define _EGA_COLOR_CYAN          0xffaaaa00
#define _EGA_COLOR_RED           0xff0000aa
#define _EGA_COLOR_VIOLET        0xffaa00aa
#define _EGA_COLOR_BROWN         0xff0055aa
#define _EGA_COLOR_GREY_LIGHT    0xffaaaaaa
#define _EGA_COLOR_GREY_DARK     0xff555555
#define _EGA_COLOR_BLUE_LIGHT    0xffff5555
#define _EGA_COLOR_GREEN_LIGHT   0xff55ff55
#define _EGA_COLOR_CYAN_LIGHT    0xffffff55
#define _EGA_COLOR_RED_LIGHT     0xff5555ff
#define _EGA_COLOR_VIOLET_LIGHT  0xffff55ff
#define _EGA_COLOR_YELLOW        0xff55ffff
#define _EGA_COLOR_WHITE         0xffffffff

void egaPalette() {
  int startColor = 0;
  palette[EGA_COLOR_BLACK+startColor] = _EGA_COLOR_BLACK;
  palette[EGA_COLOR_BLUE+startColor] = _EGA_COLOR_BLUE;
  palette[EGA_COLOR_GREEN+startColor] = _EGA_COLOR_GREEN;
  palette[EGA_COLOR_CYAN+startColor] = _EGA_COLOR_CYAN;
  palette[EGA_COLOR_RED+startColor] = _EGA_COLOR_RED;
  palette[EGA_COLOR_VIOLET+startColor] = _EGA_COLOR_VIOLET;
  palette[EGA_COLOR_BROWN+startColor] = _EGA_COLOR_BROWN;
  palette[EGA_COLOR_GREY_LIGHT+startColor] = _EGA_COLOR_GREY_LIGHT;
  palette[EGA_COLOR_GREY_DARK+startColor] = _EGA_COLOR_GREY_DARK;
  palette[EGA_COLOR_BLUE_LIGHT+startColor] = _EGA_COLOR_BLUE_LIGHT;
  palette[EGA_COLOR_GREEN_LIGHT+startColor] = _EGA_COLOR_GREEN_LIGHT;
  palette[EGA_COLOR_CYAN_LIGHT+startColor] = _EGA_COLOR_CYAN_LIGHT;
  palette[EGA_COLOR_RED_LIGHT+startColor] = _EGA_COLOR_RED_LIGHT;
  palette[EGA_COLOR_VIOLET_LIGHT+startColor] = _EGA_COLOR_VIOLET_LIGHT;
  palette[EGA_COLOR_YELLOW+startColor] = _EGA_COLOR_YELLOW;
  palette[EGA_COLOR_WHITE+startColor] = _EGA_COLOR_WHITE;
}

static void setBiosGraphicsMode(unsigned short mode) {
  union REGS regs;
  regs.w.ax = mode;
  int386(0x10, &regs, &regs);
}
 
static unsigned short getBiosGraphicsMode() {
  union REGS regs;
  regs.w.ax = 0x0f00;
  int386 (0x10, &regs, &regs);
  return regs.h.al;
}

INLINE unsigned char glGetPaletteIndex16(unsigned int rgba) {
  const int r = rgba & 255;rgba>>=8;
  const int g = rgba & 255;rgba>>=8;
  const int b = rgba & 255;rgba>>=8;
  unsigned int ld = 256*256*4;
  unsigned char ri = 0;
  for (unsigned char i = 0; i < 16; i++) {
    unsigned int pal = palette[i];
    const int r2 = (pal & 255)-r;pal >>= 8; unsigned int d = r2*r2;
    const int g2 = (pal & 255)-g;pal >>= 8; d += g2*g2;
    const int b2 = (pal & 255)-b;pal >>= 8; d += b2*b2;
    if (d < ld) {
      ld = d;
      ri = i;
    }
  }
  return ri;
}

static void glFrameBufferToScreen16Colors() {
  int x,y,i,j,b;
  unsigned char *read;
  unsigned int *write;
  unsigned int *frameBuffer = glFrameBuffer0;
  int width = glFrameBufferWidth0;
  int height = glFrameBufferHeight0;
  GLubyte *buffer = glEgaIndexBuffer;
  GLubyte *b2 = buffer;
  unsigned int *r2 = frameBuffer;

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      *b2++ = glGetPaletteIndex16(*r2++);
    }
  }

  DJGPPVGAMEMON
  for (y = 0; y < height; y++) {
    for (j = 0; j < 4; j++) {
      outp(0x3c4, 2);
      outp(0x3c5, 1<<j);
      read = &buffer[y * width];
      write = (unsigned int *)(VGARAMBASE + (width/8)*y);
      int xi = 0;
      for (x = 0; x < width/32; x++) {
        unsigned int b3 = 0;
        for (int k = 0; k < 4; k++) {
          b = 0;
          for (i = 0; i < 8; i++) {
            b |= (((read[xi])>>j) & 1)<<(7-i);
            xi++;
          }
          b3 |= b << (k*8);
        }
        *write++ = b3;
      }
    }
  }
  DJGPPVGAMEMOFF
}

static void glFrameBufferToScreenHcr() {
  int x,y,i,j,b;
  GLushort *read;
  unsigned int *write;
  unsigned int *frameBuffer = glFrameBuffer0;
  int width = glFrameBufferWidth0;
  int height = glFrameBufferHeight0;
  GLushort *buffer = (GLushort*)glEgaIndexBuffer;
  GLushort *b2 = buffer;
  unsigned int *r2 = frameBuffer;

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      unsigned int col = *r2++;
      *b2++ = glEgaHcrRemap[((col>>4) & 0xf)|((col>>8) & 0xf0)|((col>>12) & 0xf00)];
    }
  }

  DJGPPVGAMEMON
  for (y = 0; y < height; y++) {
    for (j = 0; j < 4; j++) {
      outp(0x3c4, 2);
      outp(0x3c5, 1<<j);
      read = &buffer[y * width];
      write = (unsigned int *)(VGARAMBASE + (width/8)*2*y*2);
      int xi = 0;
      for (x = 0; x < width/32*2; x++) {
        unsigned int b3 = 0;
        for (int k = 0; k < 4; k++) {
          b = 0;
          for (i = 0; i < 8; i++) {
            b |= (((read[xi/2])>>(j+((xi&1)<<2))) & 1)<<(7-i);
            xi++;
          }
          b3 |= b << (k*8);
        }
        *write++ = b3;
      }
      xi = 0;
      write = (unsigned int *)(VGARAMBASE + (width/8)*2*(y*2+1));
      for (x = 0; x < width/32*2; x++) {
        unsigned int b3 = 0;
        for (int k = 0; k < 4; k++) {
          b = 0;
          for (i = 0; i < 8; i++) {
            b |= (((read[xi/2])>>(j+((xi&1)<<2)+8)) & 1)<<(7-i);
            xi++;
          }
          b3 |= b << (k*8);
        }
        *write++ = b3;
      }
    }
  }
  DJGPPVGAMEMOFF
}

static GLvoid glRefreshEga() {
  if (glIsEga) {
    if (glEgaHCR) {
     glFrameBufferToScreenHcr();
    } else {
     glFrameBufferToScreen16Colors();
    }
  }
}

static GLvoid glDoneEga() {
  if (glIsEga) {
    setBiosGraphicsMode(glEgaGraphicsModeToRestore);
    glSetRefreshNotification(NULL);
    glSetDoneNotification(NULL);
    glIsEga = false;
    if (glFrameBuffer0 != NULL) {
      glFree(glFrameBuffer0); glFrameBuffer0 = NULL; glFrameBuffer = NULL;
    }
    
    if (glDepthBuffer0 != NULL) {
      glFree(glDepthBuffer0); glDepthBuffer0 = NULL; glDepthBuffer = NULL;
    }
    
    if (glStencilBuffer0 != NULL) {
      glFree(glStencilBuffer0); glStencilBuffer0 = NULL; glStencilBuffer = NULL;
    }
  
    if (glEgaIndexBuffer != NULL) {
      glFree(glEgaIndexBuffer); glEgaIndexBuffer = NULL;
    }
    
    if (glEgaHcrRemap != NULL) {
      glFree(glEgaHcrRemap); glEgaHcrRemap = NULL;
    }
  }
}

bool glEGA(int xRes, int yRes) {
  glIsEga = false;
  glEgaHCR = false;
  int mode = -1;
  if (xRes == 320 && yRes == 200) mode = 0x0d;
  if (xRes == 640 && yRes == 200) mode = 0x0e;
  if (xRes == 640 && yRes == 350) mode = 0x10;
  if (xRes == 640 && yRes == 480) mode = 0x12;
  if (xRes == 320 && yRes == 240) {mode = 0x012; glEgaHCR = true;}
  if (mode == -1) return false;
  glEgaGraphicsModeToRestore = getBiosGraphicsMode();
  setBiosGraphicsMode((unsigned short)mode);
  egaPalette();
  glIsEga = true;
  GLuint *frameBuffer = (GLuint*)glMalloc(xRes*yRes*sizeof(GLuint));
  GLfloat *depthBuffer = (GLfloat*)glMalloc(xRes*yRes*sizeof(GLfloat));
  GLubyte *stencilBuffer = (GLubyte*)glMalloc(xRes*yRes*sizeof(GLubyte));
  glDirect(frameBuffer, depthBuffer, stencilBuffer, xRes, yRes);
  glSetRefreshNotification(glRefreshEga);
  glSetDoneNotification(glDoneEga);
  if (!glEgaHCR) {
    glEgaIndexBuffer = (GLubyte*)glMalloc(xRes*yRes*sizeof(GLubyte));
  } else {
    glEgaIndexBuffer = (GLubyte*)glMalloc(xRes*yRes*sizeof(GLushort));
    glEgaHcrRemap = (GLushort*)glMalloc(16*16*16*sizeof(GLushort));
    int rf = (int)(glLumis[0]*256);
    int gf = (int)(glLumis[1]*256);
    int bf = (int)(glLumis[2]*256);
    for(int i0 = 0; i0 < 16; i0++) {
      int r0 = i0*255/15*4;
      for(int i1 = 0; i1 < 16; i1++) {
        int g0 = i1*255/15*4;
        for(int i2 = 0; i2 < 16; i2++) {
          int b0 = i2*255/15*4;
          int ld = 256*4*256*4*2;
          int found = 0;
          for(int c0 = 0; c0 < 16; c0++) {
            unsigned int c = palette[c0];
            int rc0 = c & 255;
            int gc0 = (c>>8) & 255;
            int bc0 = (c>>16) & 255;
            rc0 -= r0; // delta
            gc0 -= g0;
            bc0 -= b0;
            for(int c1 = 0; c1 < 16; c1++) {
              c = palette[c1];
              int rc1 = c & 255;
              int gc1 = (c>>8) & 255;
              int bc1 = (c>>16) & 255;
              rc1 += rc0;
              gc1 += gc0;
              bc1 += bc0;
              for(int c2 = 0; c2 < 16; c2++) {
                c = palette[c2];
                int rc2 = c & 255;
                int gc2 = (c>>8) & 255;
                int bc2 = (c>>16) & 255;
                rc2 += rc1;
                gc2 += gc1;
                bc2 += bc1;
                for(int c3 = 0; c3 < 16; c3++) {
                  c = palette[c3];
                  int rc3 = c & 255;
                  int gc3 = (c>>8) & 255;
                  int bc3 = (c>>16) & 255;
                  rc3 += rc2;
                  gc3 += gc2;
                  bc3 += bc2;
                  rc3 = (rc3*rf)>>8;
                  gc3 = (gc3*gf)>>8;
                  bc3 = (bc3*bf)>>8;
                  int d = rc3*rc3+gc3*gc3+bc3*bc3;
                  if (d < ld) {
                    ld = d;
                    found = c0+c1*16+c2*16*16+c3*16*16*16;
                  }
                }
              }
            }
          }
          glEgaHcrRemap[i0+i1*16+i2*16*16] = (unsigned short)found;
        }
      }
    }
  }
  return true;
}

