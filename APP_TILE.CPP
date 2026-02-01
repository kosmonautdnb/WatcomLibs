#include "GL.H"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// ASPECT RATIO IS WRONG FOR 16:9 MONITORS

// You may watch this Youtube video to get an idea of using this stb tilemap editor: https://www.youtube.com/watch?v=f9sdvwTBBOg

#define STB_TILEMAP_EDITOR_IMPLEMENTATION
void STBTE_DRAW_RECT(int,int,int,int,unsigned int);
void STBTE_DRAW_TILE(int,int,unsigned short,int,float *);
#include "stbtiled.hpp"

// No loading or saving of tilemap implemented in this file. Maybe implement a save (and/or load) key.

const int mapWidth = 10;
const int mapHeight = 10;
const int mapLayers = 5;
const int tileWidth = 32;
const int tileHeight = 32;
const int maxTiles = 30000;
#define TILESHERE 100

unsigned int tiles[TILESHERE][tileWidth*tileHeight];

void createTempTile(int index) {
  srand(index*33);
  double kx = (rand() & 1023)/1023.0;
  double ky = (rand() & 1023)/1023.0;
  for (int y = 0; y < tileHeight; y++) {
    double fy = (double)y / tileHeight;
    for (int x = 0; x < tileWidth; x++) {
      double fx = (double)x / tileWidth;
      int r = sin(fx*kx)*127+128;
      int g = sin(fy*ky)*127+128;
      int b = sin(fx*kx+fy*ky)*127+128;
      int a = sin(fx*kx*ky*fy)*127+128;
      tiles[index][x+y*tileWidth] = (r)|(g<<8)|(b<<16)|(a<<24);
    }
  }
}

void STBTE_DRAW_RECT(int x0, int y0, int x1, int y1, unsigned int color) {
  if (x0 >= glFrameBufferWidth) return;
  if (y0 >= glFrameBufferHeight) return;
  if (x1 > glFrameBufferWidth) x1 = glFrameBufferWidth;
  if (y1 > glFrameBufferHeight) y1 = glFrameBufferHeight;
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 < 0) x1 = 0;
  if (y1 < 0) y1 = 0;
  unsigned int c = ((color>>16)&0x000000ff) | ((color)&0x0000ff00) | ((color<<16)&0x00ff0000) | 0xff000000;
  for (int y = y0; y < y1; y++) {
    unsigned int *dest = &glFrameBuffer[y*glFrameBufferWidth+x0];
    for (int x = x0; x < x1; x++) {
      *dest++ = c;
    }
  }
}

void STBTE_DRAW_TILE(int x0, int y0, unsigned short id, int highlight, float *data) {
  if (id < 0 || id >= TILESHERE) return;
  for (int y = 0; y < tileHeight; y++) {
    int sy = y + y0;
    if (sy < 0) continue;
    if (sy >= glFrameBufferHeight) continue;
    for (int x = 0; x < tileWidth; x++) {
      int sx = x + x0;
      if (sx < 0) continue;
      if (sx >= glFrameBufferWidth) continue;
      unsigned int rgba1 = tiles[id][x+y*tileWidth];
      unsigned int rgba0 = glFrameBuffer[sx+sy*glFrameBufferWidth];
      int a = (rgba1 >> 24) & 255;
      int r = ((rgba1 & 255)-(rgba0 & 255))*a/255+(rgba0 & 255); rgba1>>=8; rgba0>>=8;
      int g = ((rgba1 & 255)-(rgba0 & 255))*a/255+(rgba0 & 255); rgba1>>=8; rgba0>>=8;
      int b = ((rgba1 & 255)-(rgba0 & 255))*a/255+(rgba0 & 255); rgba1>>=8; rgba0>>=8;
      switch(highlight) { // not used
        case STBTE_drawmode_deemphasize: {r/=2;g/=2;b/=2;r+=64;}break;
        case STBTE_drawmode_normal: break;
        case STBTE_drawmode_emphasize: {r*=1.5;g*=1.5;b*=1.5;if(r>255)r=255;if(g>255)g=255;if(b>255)b=255;}break;
      }
      glFrameBuffer[sx+sy*glFrameBufferWidth] = (r)|(g<<8)|(b<<16)|(255<<24);
    }
  }
}

static void paintMouse(int mouseX, int mouseY) {
  const int width = 8;
  const int height = 8;
  const char *bitmap =
  "60000000"
  "98900000"
  "98889000"
  "09888990"
  "09888889"
  "00989999"
  "00989000"
  "00099000"
  ;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      char c = bitmap[x+y*width];
      if (c != '0') {
        unsigned int color = 0xffffffff;
        if (c == '9')
          color = 0xff000000;
        if (c == '6')
          color = 0xffffffff;
        STBTE_DRAW_RECT(mouseX+x,mouseY+y,mouseX+x+1,mouseY+y+1,color);
      }
    }
  }
}

int main(int argc, const char *argv[]) {

  if (!glVesa(1024,768,16)) if (!glVesa(640,480,16)) glVGA();

  stbte_set_display(0,0,glFrameBufferWidth,glFrameBufferHeight);
  double mouseSensitivy = 320.0/glFrameBufferWidth;
  double mx = glFrameBufferWidth/2;
  double my = glFrameBufferHeight/2;
  stbte_tilemap *map = stbte_create_map(mapWidth, mapHeight, mapLayers, tileWidth, tileHeight, maxTiles);

  stbte_set_spacing(map, tileWidth, tileHeight, tileWidth+1, tileHeight+1);
  stbte_set_layername(map, 0, "Floor");

  const char *categories[4]={"Background","Floor","Stones","Flowers"};
  for (int i = 0; i < TILESHERE; i++) {
    stbte_define_tile(map, i, 255, categories[rand() % 4]);
    createTempTile(i);
  }

  bool quit = false;
  while(!quit) {
    int key = glNextKey();
    if (key == GL_VK_ESCAPE) quit = true;

    double time = glSeconds();
    static double lastTime = time;
    double timeDelta = time - lastTime;
    lastTime = time;

    double mdx, mdy;
    glNextMouseDelta(&mdx,&mdy);
    mx += mdx*mouseSensitivy;
    my += mdy*mouseSensitivy;
    if (mx < 0) mx = 0;
    if (my < 0) my = 0;
    if (mx > glFrameBufferWidth-1) mx = glFrameBufferWidth-1;
    if (my > glFrameBufferHeight-1) my = glFrameBufferHeight-1;

    GLboolean shift,ctrl,alt;
    glSpecialKeys(&shift, &ctrl, &alt);

    stbte_mouse_move(map, mx, my, shift, alt);
    static int lastMouseButtons = glMouseButtons();
    if ((glMouseButtons()^lastMouseButtons) & GL_MOUSE_BUTTON_LEFT) stbte_mouse_button(map, mx, my, 0, (glMouseButtons() & GL_MOUSE_BUTTON_LEFT) ? 1 : 0, shift, alt);
    if ((glMouseButtons()^lastMouseButtons) & GL_MOUSE_BUTTON_RIGHT) stbte_mouse_button(map, mx, my, 1, (glMouseButtons() & GL_MOUSE_BUTTON_RIGHT) ? 1 : 0, shift, alt);
    lastMouseButtons = glMouseButtons();
    //stbte_mouse_wheel(stbte_tilemap *tm, int x, int y, int vscroll); // mouse wheel currently not available

    stbte_tick(map, (float)timeDelta);
    stbte_draw(map);
    paintMouse(mx,my);
    glRefresh();
  }

  glDone();

  // return to DOS
  return 0;
}
