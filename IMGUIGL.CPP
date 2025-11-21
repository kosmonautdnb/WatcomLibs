#include "gl.h"
#include "imguigl.hpp"

extern int glFrameBufferWidth;
extern int glFrameBufferHeight;
extern int glMouseW;

double mouseX = 60, mouseY = 60;
double mouseDeltaX = 0, mouseDeltaY = 0;
double mouseSpeed = 1.0;
int mouseButtons = 0;
int currentKey = 0;
char *clipBoardString = NULL;
GLuint g_FontTexture;
ImVec2 imGuiWhitePixel();

static void drawMouseArrow(int mouseX, int mouseY) {
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
  mouseX -= glFrameBufferWidth*0.5f-glFrameBufferWidth*0.5f*glGetZoomX();
  bool first = true;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      char c = bitmap[x+y*width];
      if (c != '0') {
        unsigned int color = 0xffffffff;
        if (c == '9')
          color = 0xff000000;
        if (c == '6')
          color = 0xffffffff;
        glPixel(first,0,0,1,x+mouseX,y+mouseY,color);
        first = false;
      }
    }
  }
}

static void ImImpl_RenderDrawLists(ImDrawData* draw_data)
{ 
  glPushAttrib(0xffff);
  
  const float L = 0.0f;
  const float R = ImGui::GetIO().DisplaySize.x;
  const float B = ImGui::GetIO().DisplaySize.y;
  const float T = 0.0f;
  const float mvp[4][4] = 
  {
    { 2.0f/(R-L),	0.0f, 0.0f, 0.0f },
    { 0.0f, 2.0f/(T-B), 0.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f, 0.0f },
    { -(R+L)/(R-L), -(T+B)/(T-B), 0.0f, 1.0f },
  };
  
  // Setup orthographic projection
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glLoadMatrixf(&mvp[0][0]);
  //glOrtho(0,R,B,0,-1,1);       

  // Setup render state: alpha-blending enabled, no face culling, no depth testing
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 10.f/255.f);
  //glDisable(GL_BLEND);
  //glDisable(GL_ALPHA_TEST);

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glEnable(GL_SCISSOR_TEST);
  
  glEnable(GL_TEXTURE_2D); 
  glBindTexture(GL_TEXTURE_2D, g_FontTexture);

  int viewPort[4]; glGetIntegerv(GL_VIEWPORT,viewPort);
  int width = viewPort[2];
  int height = viewPort[3];
  ImVec2 white = imGuiWhitePixel();

  glVertex4f(0,0,0,1);

  for (int n = 0; n < draw_data->CmdListsCount; n++)
  {
      const ImDrawList* cmd_list = draw_data->CmdLists[n];
      const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();

      if (cmd_list->VtxBuffer.size() == 0)
        continue;

      ImDrawVert *vtx_data = (ImDrawVert *)&cmd_list->VtxBuffer[0];

      for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
      {
          if (pcmd->UserCallback)
          {
              pcmd->UserCallback(cmd_list, pcmd);
          }
          else
          {
              glBindTexture(GL_TEXTURE_2D, (GLuint)pcmd->TextureId);
              glScissor((int)pcmd->ClipRect.x, (int)(height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
              //glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT, idx_buffer);
                glBegin(GL_TRIANGLES);
                for (int j = 0; j < pcmd->ElemCount; j+=3) {
                  int i;
                  bool noTexture = false;
                  int flags = 1 | 2 | 4 | 8 | 16;
                  bool isTransparentColor = false;
                  for (i = j; i < j + 3; i++) {
                    ImDrawVert &a = vtx_data[idx_buffer[i]];
                    if (a.uv.x==white.x&&a.uv.y==white.y) noTexture = true;
                    if (a.pos.x >= pcmd->ClipRect.x) flags &= 31-1;
                    if (a.pos.y >= pcmd->ClipRect.y) flags &= 31-2;
                    if (a.pos.x < pcmd->ClipRect.z) flags &= 31-4;
                    if (a.pos.y < pcmd->ClipRect.w) flags &= 31-8;
                    if (((a.col>>24)&255) > 0) flags &= 31 - 16;
                    if (((a.col>>24)&255) < 250) isTransparentColor = true;
                  }
                  if (flags == 0) {
                    if (noTexture) {
                      glDisable(GL_TEXTURE_2D); 
                      glDisable(GL_ALPHA_TEST);
                      if (isTransparentColor) 
                        glEnable(GL_BLEND);
                      else
                        glDisable(GL_BLEND);
                    }
                    else {
                      glEnable(GL_TEXTURE_2D);
                      glEnable(GL_BLEND);
                      glEnable(GL_ALPHA_TEST);
                    }
        
                    for (i = j; i < j + 3; i++) {
                      ImDrawVert &a = vtx_data[idx_buffer[i]];
                      glColor4ubv((GLubyte*)&a.col);
                      glTexCoord2f(a.uv.x, a.uv.y);
                      glVertex2f(a.pos.x+0.75,a.pos.y+0.75);
                    }
                  }
                }
                glEnd();
          }
          idx_buffer += pcmd->ElemCount;
      }

  }

  glDisable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_TEXTURE_2D); 
  glDisable(GL_SCISSOR_TEST);
  glScissor(0,0,glFrameBufferWidth,glFrameBufferHeight);
  drawMouseArrow(mouseX, mouseY);
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
}

static const char* ImImpl_GetClipboardTextFn()
{
  return clipBoardString;
}

static void ImImpl_SetClipboardTextFn(const char* text)
{
  const char *text_end = text + strlen(text);
  if (clipBoardString != NULL) {delete[] clipBoardString; clipBoardString = NULL;}
  clipBoardString = new char[text_end - text + 1];
  memcpy(clipBoardString, text, text_end-text);
  clipBoardString[text_end-text] = 0;
}

static int translate(int c) {
  if (c > 255) return (c >> 8)+256;
  if (c < 32) return c + 512-32;
  return c;
}

void ImGui_CreateFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)g_FontTexture;

    // Cleanup (don't clear the input data if you want to append new fonts later)
    io.Fonts->ClearInputData();
    io.Fonts->ClearTexData();
}

void InitImGui(int w, int h)
{
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2((float)w, (float)h); // sadly this +1 is needed it seems (font rendering)
  io.DeltaTime = 1.0f/60.0f;
  io.KeyMap[ImGuiKey_Tab] = translate(GL_VK_TAB);
  io.KeyMap[ImGuiKey_LeftArrow] = translate(GL_VK_LEFT);
  io.KeyMap[ImGuiKey_RightArrow] = translate( GL_VK_RIGHT);
  io.KeyMap[ImGuiKey_UpArrow] = translate(GL_VK_UP);
  io.KeyMap[ImGuiKey_DownArrow] = translate(GL_VK_DOWN);
  io.KeyMap[ImGuiKey_Home] = translate(GL_VK_HOME);
  io.KeyMap[ImGuiKey_End] = translate(GL_VK_END);
  io.KeyMap[ImGuiKey_Delete] = translate(GL_VK_DELETE);
  io.KeyMap[ImGuiKey_Backspace] = translate(GL_VK_BACKSPACE);
  io.KeyMap[ImGuiKey_Enter] = translate(GL_VK_ENTER);
  io.KeyMap[ImGuiKey_Escape] = translate(GL_VK_ESCAPE);
  io.KeyMap[ImGuiKey_C] = translate(GL_VK_C_CTRL);
  io.KeyMap[ImGuiKey_V] = translate(GL_VK_V_CTRL);
  io.KeyMap[ImGuiKey_X] = translate(GL_VK_X_CTRL);
  //io.KeyMap[ImGuiKey_A] = GL_VK_A_CTRL;
  //io.KeyMap[ImGuiKey_Y] = GL_VK_Y_CTRL;
  //io.KeyMap[ImGuiKey_Z] = GL_VK_Z_CTRL;
  
  io.RenderDrawListsFn = ImImpl_RenderDrawLists;
  io.SetClipboardTextFn = ImImpl_SetClipboardTextFn;
  io.GetClipboardTextFn = ImImpl_GetClipboardTextFn;
  
  // Load font texture
  ImGui_CreateFontsTexture();
  
  io.IniSavingRate = -1; // disable ini saving, because we are on usb sticks which may wear out
  ImGuiStyle &s = ImGui::GetStyle();
  s.WindowRounding = 5.f;
  s.FramePadding = ImVec2(2,2);
  s.ItemSpacing = ImVec2(3,3);
  s.WindowPadding = ImVec2(3,3);
  s.Colors[ImGuiCol_WindowBg] = ImVec4(0,0,0,100.0);
  s.Colors[ImGuiCol_TitleBg] = ImVec4(0x80/255.f,0,0x40/255.f,1.f);
  s.Colors[ImGuiCol_TitleBgActive] = ImVec4(0xff/255.f,0,0x80/255.f,1.f);
  s.Colors[ImGuiCol_CloseButton] = ImVec4(0xff/255.f*0.15f,0,0x40/255.f*0.0f,1.f);
  s.Colors[ImGuiCol_Button] = ImVec4(0xff/255.f*0.45f,0,0x40/255.f*0.2f,1.f);
}

static void ImGuiKeyBoardInput() {
  currentKey = glNextKey();
  for (int i = 0; i < 512; i++)
    ImGui::GetIO().KeysDown[i]=false;
  int c = translate(currentKey);
  if (c<256) {
    ImGui::GetIO().AddInputCharacter(c);
    ImGui::GetIO().KeysDown[c]=true;
  } else  
    ImGui::GetIO().KeysDown[c]=true;
}

void ImGuiNewFrame() {
  ImGuiIO& io = ImGui::GetIO();
  static double lastTime = glSeconds();
  double time = glSeconds();
  double deltaTime = time - lastTime;
  io.DeltaTime = deltaTime > 0 ? deltaTime : 1.f/30.f;
  lastTime = time;
  mouseButtons = glMouseButtons();
  glNextMouseDelta(&mouseDeltaX, &mouseDeltaY);
  mouseX += mouseDeltaX * mouseSpeed; mouseY += mouseDeltaY * mouseSpeed;
  if (mouseX < 0) mouseX = 0;
  if (mouseY < 0) mouseY = 0;
  if (mouseX > glFrameBufferWidth-1) mouseX = glFrameBufferWidth-1;
  if (mouseY > glFrameBufferHeight-1) mouseY = glFrameBufferHeight-1;
  io.MousePos = ImVec2(mouseX,mouseY);
  io.MouseDown[0] = mouseButtons & 1;
  io.MouseDown[1] = mouseButtons & 2;
  io.MouseWheel = (signed char)((mouseButtons>>8)&0xff);
  ImGuiKeyBoardInput();
  ImGui::NewFrame();
}

static int XRES = 640;
static int YRES = 480;

void ImGuiDemo() {
  glVesa(XRES,YRES,32);

  InitImGui(XRES, YRES);

  do
  {
    ImGuiNewFrame();

    // 2) ImGui usage
    static bool show_test_window = false;
    static bool show_another_window = false;
    static float f;
    ImGui::Text("Hello, world!");
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    if (ImGui::Button("Test Window")) show_test_window = !show_test_window;
    if (ImGui::Button("Another Window")) show_another_window = !show_another_window;
    
    // Calculate and show framerate
    #define BUCKETS 10
    static float ms_per_frame[BUCKETS] = { 0 };
    static int ms_per_frame_idx = 0;
    static float ms_per_frame_accum = 0.0f;
    ms_per_frame_accum -= ms_per_frame[ms_per_frame_idx];
    ms_per_frame[ms_per_frame_idx] = ImGui::GetIO().DeltaTime * 1000.0f;
    ms_per_frame_accum += ms_per_frame[ms_per_frame_idx];
    ms_per_frame_idx = (ms_per_frame_idx + 1) % BUCKETS;
    const float ms_per_frame_avg = ms_per_frame_accum / BUCKETS;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", ms_per_frame_avg, 1000.0f / ms_per_frame_avg);
    
    if (show_test_window)
    {     
      // More example code in ShowTestWindow()
      ImGui::ShowTestWindow(&show_test_window);
    }
    
    if (show_another_window)
    {
      ImGui::Begin("Another Window", &show_another_window, ImVec2(200,100));
      ImGui::Text("Hello");
      static char buffer[100];
      ImGui::InputText("Hello", buffer, 100);
      ImGui::End();
    }

    glClearColor(10.f/255.f,40.f/255.f,30.f/255.f,1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    glRefresh();
  } while(currentKey!=GL_VK_ESCAPE);
  glDone();
}