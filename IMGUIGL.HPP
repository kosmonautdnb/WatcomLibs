#ifndef __IMGUIGL_HPP__
#define __IMGUIGL_HPP__

#include "imgui.h"

#define __BEGINWINDOW(__NAME__) ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0xff/255.f,0x00/255.f,0x80/255.f,0x38/255.f)); ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0x08/255.f,0x02/255.f,0x04/255.f,1.0)); ImGui::Begin(__NAME__,NULL,ImVec2(0,0),0.8);
#define __ENDWINDOW ImGui::End(); ImGui::PopStyleColor(); ImGui::PopStyleColor();

void InitImGui(int w, int h);
void ImGuiNewFrame();
void ImGuiDemo();

#endif //__IMGUIGL_HPP__
