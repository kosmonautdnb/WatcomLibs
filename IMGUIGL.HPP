#ifndef __IMGUIGL_HPP__
#define __IMGUIGL_HPP__

#include "imgui.h"

extern double mouseX, mouseY;
extern double mouseDeltaX, mouseDeltaY;
extern double mouseSpeed;
extern int mouseButtons;
extern int currentKey;

void InitImGui(int w, int h);
void ImGuiNewFrame();
void ImGuiDemo();

#endif //__IMGUIGL_HPP__
