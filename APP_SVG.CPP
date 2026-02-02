#include "GL.H"
#define NANOSVG_IMPLEMENTATION
#include "NANOSVG.HPP"
#define NANOSVGRAST_IMPLEMENTATION
#include "NSVGRAST.HPP"
#include <stdio.h>
#include <stdlib.h>

//#define ASPECT (16.0/9.0)
#define ASPECT (4.0/3.0) // NANOSVG doesn't have an aspect ratio correction

int main(int argc, const char *argv[]) {

  printf("SVG Picture Viewer\n");
  printf("-------------------------\n");
  if (argc != 2) {
    printf("usage: app_svg.exe <image.svg>\n");
    printf("the source folder should contain a test.svg\n");
    exit(0);
  }
  const char *fileName = argv[1];
  printf("loading image <%s>\n", fileName);

  NSVGimage* image;
  NSVGrasterizer* rast = nsvgCreateRasterizer();
  image = nsvgParseFromFile(fileName, "px", 96); // dpi(dots per inch) is 96px(pixels)
  if (image == NULL) {
    printf("some problem with loading the svg\n");
    exit(0);
  }

  if (!glVesa(640,480,32)) glVGA();
  glRefresh();

  const double aspect = (double)ASPECT * glFrameBufferHeight / glFrameBufferWidth;

  int w = glFrameBufferWidth;
  int h = glFrameBufferHeight;
  unsigned int* img = (unsigned int*)malloc(w*h*4);
  nsvgRasterize(rast, image, 0,0,1/aspect, (unsigned char*)img, w, h, w*4);
  nsvgDelete(image);

  bool quit = false;
  while(!quit) {
    int key = glNextKey();
    if (key == GL_VK_ESCAPE) 
      quit = true;

    for (int y = 0; y < glFrameBufferHeight; y++) {
      int iy = y/aspect;
      for (int x = 0; x < glFrameBufferWidth; x++) {
        int ix = x;
        if (((unsigned int)ix>=w)||((unsigned int)iy>=h)) 
          glFrameBuffer[x+y*glFrameBufferWidth] = (x*333)^(y*777);
        else
          glFrameBuffer[x+y*glFrameBufferWidth] = img[ix+iy*w];
      }
    }

    glRefresh();
  }

  glDone();

  return 0;
}
