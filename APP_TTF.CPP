#include "GL.H"
#include "TRUETYPE.HPP"
#include <stdlib.h>
#include <stdio.h>

#define ASPECT (16.0/9.0)

int main(int argc, const char *argv[]) {

  printf("TrueType(TTF) font viewer\n");
  printf("-------------------------\n");
  if (argc != 2) {
    printf("usage: app_ttf.exe <font.ttf>\n");
    printf("the source folder should contain a libesans.ttf\n");
    exit(0);
  }
  const char *fileName = argv[1];

  //if (!glVesa(640,480,16)) 
  glVGA();
  glRefresh();

  GLuint fontTexture[4];
  GLfloat sizes[4];
  for (int i = 0; i < 4; i++) {sizes[i] = glFrameBufferWidth/(26-i*4); fontTexture[i] = stb_initfont(i, fileName, sizes[i]);}
   
  // maybe code an INTER_AREA downsampler for it.
  const double aspect = 1.0; // Aspect Ratio is currently not working with baked fonts
  //const double aspect = (double)ASPECT * glFrameBufferHeight / glFrameBufferWidth;

  bool quit = false;
  while(!quit) {
    int key = glNextKey();
    if (key == GL_VK_ESCAPE) 
      quit = true;

    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,glFrameBufferWidth*aspect,glFrameBufferHeight,0,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    float posY = 0;
    for (int i = 0; i < 4; i++) {
      glBindTexture(GL_TEXTURE_2D,fontTexture[i]);    
      stb_print(i, 0, posY, "Jumpy kids vex dwarves with explosive firework display.", 0xff000000, 0, -1);
      posY += sizes[i];
    }
    glDisable(GL_TEXTURE_2D);

    glRefresh();
  }

  glDone();

  return 0;
}
