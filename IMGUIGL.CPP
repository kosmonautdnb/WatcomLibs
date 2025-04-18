#include "c:/_OPENGL_/gl.h"
#include "image.hpp"
#include "imguigl.hpp"

extern int glFrameBufferWidth;
extern int glFrameBufferHeight;
extern double glMonitorAspectRatio;

double mouseX = 60, mouseY = 60;
double mouseDeltaX = 0, mouseDeltaY = 0;
int mouseButtons = 0;
int currentKey = 0;
char *clipBoardString = NULL;

static GLuint fontTex;

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
  mouseX -= glFrameBufferWidth*0.5f-glFrameBufferWidth*0.5f*glMonitorAspectRatio;
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


static void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
{ 
  size_t total_vtx_count = 0;
  int n;
  for (n = 0; n < cmd_lists_count; n++)
    total_vtx_count += cmd_lists[n]->vtx_buffer.size();
  if (total_vtx_count == 0)
    return;
  
  int read_pos_clip_rect_buf = 0;  // offset in 'clip_rect_buffer'. each PushClipRect command consume 1 of those.
  
  ImVector<ImVec4> clip_rect_stack;
  clip_rect_stack.push_back(ImVec4(-9999,-9999,+9999,+9999));
  
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
  
  //glBindBuffer(GL_ARRAY_BUFFER, vbo);
  //glBindVertexArray(vao);
  static unsigned char *buffer_data2 = NULL;
  if (buffer_data2 != NULL) {delete[] buffer_data2; buffer_data2 = NULL;}
  buffer_data2 = (unsigned char*)(new ImDrawVert[total_vtx_count]);
  ImDrawVert *vtx = (ImDrawVert*)buffer_data2;
  unsigned char *buffer_data = (unsigned char*)buffer_data2;
  //glBufferData(GL_ARRAY_BUFFER, total_vtx_count * sizeof(ImDrawVert), NULL, GL_STREAM_DRAW);
  //unsigned char* buffer_data = (unsigned char*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  //if (!buffer_data)
  //  return;
  int vtx_consumed = 0;

  for (n = 0; n < cmd_lists_count; n++)
  {
    const ImDrawList* cmd_list = cmd_lists[n];
    if (!cmd_list->vtx_buffer.empty())
    {
      memcpy(buffer_data, &cmd_list->vtx_buffer[0], cmd_list->vtx_buffer.size() * sizeof(ImDrawVert));
      buffer_data += cmd_list->vtx_buffer.size() * sizeof(ImDrawVert);
      vtx_consumed += cmd_list->vtx_buffer.size();
    }
  }
  //glUnmapBuffer(GL_ARRAY_BUFFER);
  
  //glUseProgram(shaderProgram);

  // Setup orthographic projection
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glLoadMatrixf(&mvp[0][0]);

  //glUniformMatrix4fv(uniMVP, 1, GL_FALSE, &mvp[0][0]);
  
  // Setup render state: alpha-blending enabled, no face culling, no depth testing
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 10.f/255.f);
  //glDisable(GL_BLEND);
  //glDisable(GL_ALPHA_TEST);

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  
  glEnable(GL_TEXTURE_2D); 
  glBindTexture(GL_TEXTURE_2D, fontTex);
  
  vtx_consumed = 0; // offset in vertex buffer. each command consume ImDrawCmd::vtx_count of those
  bool clip_rect_dirty = true;

  int viewPort[4]; glGetIntegerv(GL_VIEWPORT,viewPort);

  for (n = 0; n < cmd_lists_count; n++)
  {
    const ImDrawList* cmd_list = cmd_lists[n];
    if (cmd_list->commands.empty() || cmd_list->vtx_buffer.empty())
      continue;
    const ImDrawCmd* pcmd = &cmd_list->commands.front();
    const ImDrawCmd* pcmd_end = &cmd_list->commands.back();
    int clip_rect_buf_consumed = 0;  // offset in cmd_list->clip_rect_buffer. each PushClipRect command consume 1 of those.
    while (pcmd <= pcmd_end)
    {
      const ImDrawCmd& cmd = *pcmd++;
      switch (cmd.cmd_type)
      {
        case ImDrawCmdType_DrawTriangleList:
        {
          float *k = (float*)&clip_rect_stack.back();
          if (clip_rect_dirty)
          {
            //glUniform4fv(uniClipRect, 1, (float*)&clip_rect_stack.back());
            // here we need glScissor
            glScissor(k[0],viewPort[3]-k[3],k[2]-k[0],k[3]-k[1]);
            clip_rect_dirty = false;
          }
          //glDrawArrays(GL_TRIANGLES, vtx_consumed, cmd.vtx_count);
          glBegin(GL_TRIANGLES);
          for (int j = vtx_consumed; j < vtx_consumed + cmd.vtx_count; j+=3) {
            int i;
            bool noTexture = false;
            int flags = 1 | 2 | 4 | 8;
            for (i = j; i < j + 3; i++) {
              ImDrawVert &a = vtx[i];
              if (a.uv.x==0&&a.uv.y==0) noTexture = true;
              if (a.pos.x >= k[0]) flags &= 15-1;
              if (a.pos.y >= k[1]) flags &= 15-2;
              if (a.pos.x < k[2]) flags &= 15-4;
              if (a.pos.y < k[3]) flags &= 15-8;
            }
            if (flags == 0) {
              if (noTexture) 
                glDisable(GL_TEXTURE_2D); 
              else 
                glEnable(GL_TEXTURE_2D);
  
              for (i = j; i < j + 3; i++) {
                ImDrawVert &a = vtx[i];
                glColor4ubv((GLubyte*)&a.col);
                glTexCoord2f(a.uv.x, a.uv.y);
                glVertex2f(a.pos.x,a.pos.y);
              }
            }
          }
          glEnd();
          vtx_consumed += cmd.vtx_count;
        }
        break;
        
        case ImDrawCmdType_PushClipRect:
          clip_rect_stack.push_back(cmd_list->clip_rect_buffer[clip_rect_buf_consumed++]);
          clip_rect_dirty = true;
        break;
        
        case ImDrawCmdType_PopClipRect:
          clip_rect_stack.pop_back();
          clip_rect_dirty = true;
        break;
      }
    }
  }
  glDisable(GL_SCISSOR_TEST);
  glScissor(0,0,glFrameBufferWidth,glFrameBufferHeight);
  drawMouseArrow(mouseX, mouseY);
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}

static const char* ImImpl_GetClipboardTextFn()
{
  return clipBoardString;
}

static void ImImpl_SetClipboardTextFn(const char* text, const char* text_end)
{
  if (!text_end)
    text_end = text + strlen(text);
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

void InitImGui(int w, int h)
{
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2((float)w, (float)h);
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
  glGenTextures(1, &fontTex);
  glBindTexture(GL_TEXTURE_2D, fontTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  const void* png_data;
  unsigned int png_size;
  ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
  int tex_x, tex_y, tex_comp;
  void* tex_data = stbi_load_from_memory((const unsigned char*)png_data, (int)png_size, &tex_x, &tex_y, &tex_comp, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_x, tex_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
  stbi_image_free(tex_data);
  
  ImGui::GetStyle().WindowRounding = 5.f;
  ImGui::GetStyle().FramePadding = ImVec2(2,1);
  ImGui::GetStyle().ItemSpacing = ImVec2(3,3);
  ImGui::GetStyle().WindowPadding = ImVec2(3,3);
  io.IniSavingRate = -1; // disable ini saving, because we are on usb sticks which may wear out
  ImGuiStyle &s = ImGui::GetStyle();
  s.Colors[ImGuiCol_TitleBg] = ImVec4(0xff/255.f,0,0x40/255.f,0x38/255.f);
  s.Colors[ImGuiCol_CloseButton] = ImVec4(0xff/255.f*0.15f,0,0x40/255.f*0.0f,0xff/255.f);
  s.Colors[ImGuiCol_Button] = ImVec4(0xff/255.f*0.45f,0,0x40/255.f*0.2f,0xff/255.f);
}

static void ImGuiKeyBoardInput() {
  currentKey = glNextKey();
  for (int i = 0; i < 256; i++)
    ImGui::GetIO().KeysDown[256+i]=false;
  int c = translate(currentKey);
  if (c<256) 
    ImGui::GetIO().AddInputCharacter(c);
  else  
    ImGui::GetIO().KeysDown[c]=true;
}

void ImGuiNewFrame() {
  ImGuiIO& io = ImGui::GetIO();
  static double lastTime = glSeconds();
  double time = glSeconds();
  double deltaTime = time - lastTime;
  io.DeltaTime = deltaTime > 0 ? deltaTime : 1.f/30.f;
  lastTime = time;
  glNextMouseDelta(&mouseDeltaX, &mouseDeltaY);
  mouseX += mouseDeltaX; mouseY += mouseDeltaY;
  if (mouseX < 0) mouseX = 0;
  if (mouseY < 0) mouseY = 0;
  if (mouseX > glFrameBufferWidth-1) mouseX = glFrameBufferWidth-1;
  if (mouseY > glFrameBufferHeight-1) mouseY = glFrameBufferHeight-1;
  io.MousePos = ImVec2(mouseX,mouseY);
  mouseButtons = glMouseButtons();
  io.MouseDown[0] = mouseButtons & 1;
  io.MouseDown[1] = mouseButtons & 2;
  io.MouseWheel = 0;
  ImGuiKeyBoardInput();
  ImGui::NewFrame();
}

static int XRES = 640;
static int YRES = 480;

void ImGuiDemo() {
  glVesa(XRES,YRES,32);
  //glSetMonitorAspectRatio(16.0/9.0);

  InitImGui(XRES, YRES);

  do
  {
    ImGuiNewFrame();

    // 2) ImGui usage
    static bool show_test_window = true;
    static bool show_another_window = true;
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
      ImGui::SetNewWindowDefaultPos(ImVec2(650, 20));		// Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
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