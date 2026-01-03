#include "gl.h"
#include "ImGui.h"
#include "ImGuiGl.hpp"
#include <stdio.h> // for printf

#define XRES 640
#define YRES 480

int main(int argc, const char *argv[]) {

  glWatcomPrecisionTimer(GL_TRUE);

  const bool pureVGA = false;
  if (pureVGA) {
    glVGA();
  } else {
    if (!glVesa(XRES,YRES,16)) {
      printf("No Vesa %dx%dx16 found.\n", XRES, YRES);
      if (!glVGA())
      {
        printf("No VGA device found.\n");
        exit(0);
      }
    }
  }

  InitImGui(glFrameBufferWidth,glFrameBufferHeight);

  bool quit = false;
  while(!quit) {

    if (currentKey == GL_VK_ESCAPE) quit = true;

    glClearColor(0.6,0.8,1.0,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    ImGuiNewFrame();

    ImGui::ShowTestWindow();
    ImGui::ShowMetricsWindow();

    ImGui::Begin("Window1");
    if (ImGui::Button("Exit##Exit1")) quit = true;
    ImGui::Checkbox("Exit##Exit2",&quit);
    ImGui::SameLine();
    ImGui::Checkbox("Exit##Exit3",&quit);
    ImGui::Text("MouseX %d,MouseY %d",(int)mouseX,(int)mouseY);
    ImGui::Text("MouseDeltaX %d,MouseDeltaY %d",(int)mouseDeltaX,(int)mouseDeltaY);
    ImGui::Text("MouseButtons %d",mouseButtons);
    ImGui::Text("CurrentKey %d",(int)currentKey);
    ImGui::Text("Time %f",glSeconds());
    ImGui::End();

    ImGui::Render();
      
    glRefresh();
  }

  glDone();
  return 0;
}
