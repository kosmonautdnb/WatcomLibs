#include "GL.H" // WatcomGL
#include "CHARSET.HPP" // a simple text rendering class with a very simple font
#include "QUATERNN.HPP" // quaternions for rotation
#include "VECTOR.HPP" // 3 component vector arithmetic with the 4th(w) value handled differently
#include "MATRIX.HPP" // 4 by 4 matrix class
#include "KEYMTRIX.HPP" // multi-key input
#define CGLTF_IMPLEMENTATION // needed in one single file before including "CGLTFA.HPP"
#include "CGLTFA.HPP" // the glb/gltf loader
#include <math.h> // for fmod()

#define FOV 90.0 // field of view, a human has around 50-70 or something here
#define ASPECT (16.0/9.0) // aspect, current monitors have 16:9 old DOS monitors have 4:3
#define NEARPLANE 0.001 // nearplane (has to be not directly 0)
#define FARPLANE 1000.0 // farplane it's granulary also stems from the near plane
#define GLTFTEXTURESCALE 0.5 // at loading of the gltf the textures are scaled down by this factor (to prevent memory overflow)

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

int main(int argc, const char *argv[]) {

  // startup
  printf("GLB/GLTF File Viewer\n");
  printf("-------------------------\n");
  if (argc != 2) {
    printf("usage: app_gltf.exe <filename.glb>\n");
    printf("in the source folder there is \"vroid.glb\" for testing.\n");
    exit(0);
  }
  const char *fileName = argv[1];

  // loading glb/gltf file, if used for non glb it will have no textures loaded
  GLTFA_File *gltf = new GLTFA_File();
  printf("Loading <%s>\n",fileName);
  if (!gltf->load(fileName, GLTFTEXTURESCALE)) {
    printf("Couldn't load <%s>\n",fileName);
    exit(0);
  }

  // only for WatcomC to have a fine granular time without stuttering
  glWatcomPrecisionTimer(GL_TRUE);

  // initialize basic graphics mode, maybe try glVesa(640,480,16) or similar here
  glVGA();
  glRefresh();

  // for multi-key input
  installKeyboardHandler();

  // input speed values
  double cursorSpeed = 10.0;
  double mouseSpeed = 0.5;

  // startup camera position and viewing angle
  Quaternion cameraRotation;
  cameraRotation.yawPitchRoll(0,-3.1415927*0.25,0);
  Vector cameraPosition(0,10.0,10.0);

  // loading "GRID" texture
  unsigned char floorTextureData[4]={0,0xff,0xff,0};
  GLuint floorTexture;
  glGenTextures(1, &floorTexture);
  glBindTexture(GL_TEXTURE_2D, floorTexture);
  glTexImage2D(GL_TEXTURE_2D,0,GL_ALPHA,2,2,0,GL_ALPHA,GL_UNSIGNED_BYTE,floorTextureData);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // default gl settings
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  bool quit = false;
  while(!quit) {
    // call dos key get interrupt, mainly to have the computer not beeping whilst inputting multi-keys
    int key = glNextKey();
    if (key == GL_VK_ESCAPE) 
      quit = true;

    // timing related
    double timePoint = glSeconds();
    static double lastTimePoint = timePoint;
    double timeDelta = timePoint - lastTimePoint;
    lastTimePoint = timePoint;

    // mouse related
    double mdx, mdy;
    glNextMouseDelta(&mdx,&mdy);

    // for shift press
    GLboolean alt,shift,ctrl;
    glSpecialKeys(&shift, &ctrl, &alt);

    // camera rotation with mouse (using quaternions)
    if (!shift) {
      Quaternion ym; ym.yawPitchRoll(0,-mdy * mouseSpeed * timeDelta,0); 
      Quaternion xm; xm.yawPitchRoll(-mdx * mouseSpeed * timeDelta,0,0); 
      cameraRotation = cameraRotation * ym;
      cameraRotation = cameraRotation * xm;
    } else {
      Quaternion xm; xm.yawPitchRoll(0,0,-mdx * mouseSpeed * timeDelta); 
      cameraRotation = cameraRotation * xm;
    }
    cameraRotation = normalize(cameraRotation);

    // camera movement using multi-keys
    if (isKeyPressed(SCANCODE_W)) cameraPosition += cameraRotation.rotateVector(Vector(0,0,-1)) * timeDelta * cursorSpeed;
    if (isKeyPressed(SCANCODE_S)) cameraPosition += cameraRotation.rotateVector(Vector(0,0,1)) * timeDelta * cursorSpeed;
    if (isKeyPressed(SCANCODE_D)) cameraPosition += cameraRotation.rotateVector(Vector(1,0,0)) * timeDelta * cursorSpeed;
    if (isKeyPressed(SCANCODE_A)) cameraPosition += cameraRotation.rotateVector(Vector(-1,0,0)) * timeDelta * cursorSpeed;
    if (isKeyPressed(SCANCODE_E)) cameraPosition += cameraRotation.rotateVector(Vector(0,1,0)) * timeDelta * cursorSpeed;
    if (isKeyPressed(SCANCODE_Q)) cameraPosition += cameraRotation.rotateVector(Vector(0,-1,0)) * timeDelta * cursorSpeed;


    // clear screen and depth buffer
    glClearColor(0,0,0,1);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // setup perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOV,ASPECT,NEARPLANE,FARPLANE);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // apply camera to OpenGL
    glMultMatrixd(cameraRotation.inverse().rotationMatrix().m);
    glTranslatef(-cameraPosition.x,-cameraPosition.y,-cameraPosition.z);

    // animate gltf (glb) if it has animations
    if (!gltf->gltfAnimations.empty()) {
      double timemin = gltf->gltfAnimations[0]->channels[0].timeMin;
      double timemax = gltf->gltfAnimations[0]->channels[0].timeMax;
      if (timemax-timemin>0) {
        gltf->applyAnimation(fmod(timePoint,timemax-timemin)+timemin,gltf->gltfAnimations[0]);
      }
    }
    // paint the gltf
    gltf->drawScene(gltf->defaultScene);

    // paint checkered floor
    const float TEXMAXX = 100.0;
    const float TEXMAXY = 100.0;
    const float GRIDCELLSIZE = 10.0;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,floorTexture);
    glDisable(GL_CULL_FACE);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0.5);
    glBegin(GL_QUADS);
    glColor4f(0.25,0.25,0.25,1); // grid color
    glTexCoord2f(TEXMAXX/GRIDCELLSIZE,0); glVertex3f(TEXMAXX/2,0,-TEXMAXY/2);
    glTexCoord2f(0,0); glVertex3f(-TEXMAXX/2,0,-TEXMAXY/2);
    glTexCoord2f(0,TEXMAXY/GRIDCELLSIZE); glVertex3f(-TEXMAXX/2,0,+TEXMAXY/2);
    glTexCoord2f(TEXMAXX/GRIDCELLSIZE,TEXMAXY/GRIDCELLSIZE); glVertex3f(+TEXMAXX/2,0,+TEXMAXY/2);
    glEnd();                                          
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);

    // output text
    printAt(0,6*0,"Use mouse(+Shift) to rotate.");
    printAt(0,6*1,"Use W,A,S,D and Q,E to move.");

    // update screen with OpenGL content
    glRefresh();
  }

  // cleanup
  uninstallKeyboardHandler();
  glDone();
  delete gltf;

  // return to DOS
  return 0;
}
