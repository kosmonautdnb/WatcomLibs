#include "GL.H" // WatcomGL
#include "CHARSET.HPP" // a simple text rendering class with a very simple font
#include "IMAGE.HPP" // the image loading stuff

#define ASPECT (16.0/9.0)

// simple text output on graphics screen
void printAt(int x, int y, const char *text) {
  // save matrices
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  // setup screen buffer matrix
  glOrtho(0,glFrameBufferWidth,glFrameBufferHeight,0,-1,1);
  // non standard WatcomGLfunction
  glDrawText(false, x, y, 0, 1.f, text, 0xffffffff, 0, 0);
  // restore matrices
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

char buffer[256];

int main(int argc, const char *argv[]) {

  // startup
  printf("OpenGL Picture Viewer\n");
  printf("-------------------------\n");
  if (argc != 2) {
    printf("usage: app_img.exe <imageFileName>\n");
    printf("supported formats: JPEG,PNG,BMP,PSD,TGA,GIF,HDR,PIC,PNM\n");
    printf("the source folder should contain a test.png\n");
    exit(0);
  }
  const char *fileName = argv[1];
  printf("loading image <%s>\n", fileName);
  RGBAImage img = RGBAImage::fromFile(fileName);
  if (img.data == NULL) {
    printf("couldn't load / not supported image <%s>.\n", fileName);
    exit(0);
  }

  if (!glVesa(640,480,32)) glVGA();
  glRefresh();

  const double aspect = (double)ASPECT * glFrameBufferHeight / glFrameBufferWidth;

  // loading "GRID" texture
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img.width,img.height,0,GL_RGBA,GL_UNSIGNED_BYTE,img.data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  double zoom = 1.0;
  double xp = glFrameBufferWidth*aspect/2, yp = glFrameBufferHeight/2;   

  bool quit = false;
  while(!quit) {
    int key = glNextKey();
    if (key == GL_VK_ESCAPE) 
      quit = true;

    if (key == '-') {zoom /= 1.5;}
    if (key == '+') {zoom *= 1.5;}
    
    // mouse related
    double mdx, mdy;
    glNextMouseDelta(&mdx,&mdy);
    xp += mdx;
    yp += mdy;
    if (xp < -img.width*zoom*0.5) xp = -img.width*zoom*0.5;
    if (yp < -img.height*zoom*0.5) yp = -img.height*zoom*0.5;
    if (xp > glFrameBufferWidth*aspect+img.width*zoom*0.5) xp = glFrameBufferWidth*aspect+img.width*zoom*0.5;
    if (yp > glFrameBufferHeight+img.height*zoom*0.5) yp = glFrameBufferHeight+img.height*zoom*0.5;

    // clear screen and depth buffer
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    // setup perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);
    glColor4f(0.5,0.5,0.5,1); 
    glVertex2f(1,1);
    glVertex2f(-1,1);
    glColor4f(0.0,0.0,0.0,1); 
    glVertex2f(-1,-1);
    glVertex2f(1,-1);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,glFrameBufferWidth * aspect,glFrameBufferHeight,0,-1,1);


    glTranslatef(xp,yp,0);
    glScalef(zoom,zoom,1);      
    glTranslatef(-img.width/2,-img.height/2,0);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glColor4f(1,1,1,1);
    glTexCoord2f(1,0); glVertex2f(img.width,0);
    glTexCoord2f(0,0); glVertex2f(0,0);
    glTexCoord2f(0,1); glVertex2f(0,img.height);
    glTexCoord2f(1,1); glVertex2f(img.width,img.height);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // output text
    printAt(0,6*0,"Use mouse to scroll.");
    printAt(0,6*1,"Use + and - to zoom.");
    sprintf(buffer,"zoom:%0.2f width:%d,height:%d <%s>",zoom,img.width,img.height,fileName);
    printAt(0,6*2,buffer);
    printAt(0,6*3,"monitor aspect: 16:9\n");


    // update screen with OpenGL content
    glRefresh();
  }

  // cleanup
  glDone();

  // return to DOS
  return 0;
}
