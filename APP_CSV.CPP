// this is just an example, not a perfectly coded CSV viewer
// it may get slow if you have more than 10000 entries or a slow computer
// maybe your desktop doesn't support 1024x768x16 as resolution which is needed here.
// you may press escape to exit the app.
// it maybe has some bugs left.. just some sort of very simple inhouse csv reader by Puls4r..
#include "ARRAY.HPP"
#include "STRING.HPP"
#include "UTF8.HPP"
#include "GL.H"
#include "IMGUI.H"
#include "IMGUIGL.HPP"

Array<char> csvData;
Array<int> begins;
Array<int> ends;
Array<String> particles;
int maxLen;
char searchText[1024];
int from = 0;
int to = 0;
int header = 0;

void addParticle(int begin, int end) {
  int i0 = begin; int i1 = end;
  {for (int i = begin; i < end; i++) {
    if (csvData[i] == '"') {
      if (i > begin && csvData[i-1] == '\\') continue;
      i0 = i+1; break;
    }
  }}

  {for (int i = end - 1; i >= begin; i--) {
    if (csvData[i] == '"') {
      if (i > begin && csvData[i-1] == '\\') continue;
      i1 = i; break;
    }
  }}

  if (i1 == i0) {particles.push_back("");return;}
  if (i1 < i0) return;
  static Array<char> p; p.resize(i1-i0+1); memcpy(&p[0],&csvData[i0],(i1-i0)*sizeof(char)); p[i1-i0] = 0x00;

  utf8_int8_t *k = (utf8_int8_t *)&p[0]; int l = (int)utf8len(k);
  String s; s.resize(l);
  for (int j = 0; j < l; j++) {
    utf8_int32_t cp;
    k = utf8codepoint(k, &cp);
    if (cp > 128) cp = '?';
    s[j] = (char)(cp & 255);
  }
  particles.push_back(s);
}

bool isEmpty(const String &k) {
  for (int j = 0; j < (int)k.size(); j++) if (k[j] != ' ' && k[j] != 0x0d && k[j] != 0x0a && k[j] != '\t') return false;
  return true;
}

void processCSV() {
  int len = (int)csvData.size();
  char *source = &csvData[0];
  int begin = 0;
  int end = 0;
  bool inString = false;
  begins.push_back(0);
  for (int i = 0; i < len+1; i++) {
    char c = (char)(i < len ? source[i] : 0x0d);
    if (!inString) {
      if (c == 0x0d || c == 0x0a) {
        end = i; addParticle(begin,end);
        ends.push_back((int)particles.size()); if (i != len) begins.push_back((int)particles.size());
        if (i+1<len) {
          char d = source[i+1];
          if (c == 0x0d && d == 0x0a) i++;
          if (c == 0x0a && d == 0x0d) i++;
        }
        begin = i + 1;
        continue;
      }
      if (c == ',' || c == ';') {
        end = i; addParticle(begin,end);
        begin = i + 1;
        continue;
      }
    }
    if (c == '"') {
      if (i > 0 && source[i-1] =='\\') continue;
      inString = !inString;
    }
  }
  if (begins.size() != ends.size()) exit(0);

  if (!begins.empty()) {
    // one line = 1 begin at start (with 0) + another one
    begins.resize((int)begins.size()-1);
    ends.resize((int)ends.size()-1);
  }

  maxLen = 0;
  for (int l = 0; l < (int)begins.size(); l++) {
    int k = ends[l]-1;
    while(k >= begins[l]) {
      if (!isEmpty(particles[k])) {
        int k2 = k+1 - begins[l];
        if (k2 > maxLen) maxLen = k2;
      }
      k--;
    }
  }
}

void toolTipHere(const String &s) {
  if (ImGui::IsItemHovered()) {
    ImGui::SetNextWindowSize(ImVec2(glFrameBufferWidth/3,glFrameBufferHeight/5));
    ImGui::BeginTooltip();
    ImGui::TextWrapped("%s",s.c_str());
    ImGui::EndTooltip();
  }
}

int main(int argc, const char *argv[]) {

  if (argc != 2) {
    printf("usage: app_csv.exe <utf8csvfile>\n");
    printf("There should be an example .csv in the folder called test.csv\n");
    return 0;
  }

  printf("Loading file: <%s>\n",argv[1]);
  FILE *in = fopen(argv[1],"rb");
  if (in == NULL) {
    printf("File not found: <%s>\n", argv[1]);
    return 0;
  }
  fseek(in,0,SEEK_END);
  csvData.resize(ftell(in));
  fseek(in,0,SEEK_SET);
  fread(&csvData[0],1,csvData.size(),in);
  fclose(in);
  printf("loading done..\n");
  printf("Processing csv...\n");
  processCSV();
  from = 1;
  to = (int)begins.size();

  if (!glVesa(1024,768,16)) {
    printf("Could not init graphics mode 1024x768 with 16bit color depth.\n");
    return 0;
  }
  glRefresh();
  InitImGui(glFrameBufferWidth,glFrameBufferHeight);

  glClearColor(0.6,0.8,1.0,0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  bool quit = false;
  while(!quit) {

    ImGuiNewFrame();

    if (currentKey == GL_VK_ESCAPE || currentKey == GL_VK_END) quit = true;

    double scrollAdd = 0;
    if (currentKey == GL_VK_DOWN) scrollAdd = glFrameBufferHeight/2;
    if (currentKey == GL_VK_UP) scrollAdd = -glFrameBufferHeight/2;
    if (currentKey == GL_VK_PAGEDOWN) scrollAdd = glFrameBufferHeight;
    if (currentKey == GL_VK_PAGEUP) scrollAdd = -glFrameBufferHeight;

    int headerSize = 40;

    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImVec2(glFrameBufferWidth,headerSize));
    ImGui::Begin("CSVHEADER",NULL,ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse);
      ImGui::PushItemWidth(300);
      ImGui::Text("Search for"); ImGui::SameLine(); ImGui::InputText("##SEARCHFOR",searchText,1024);
      ImGui::PopItemWidth();
      ImGui::PushItemWidth(7*20);
      ImGui::SameLine(); ImGui::Text("From"); ImGui::SameLine(); ImGui::InputInt("##FROM",&from);
      ImGui::SameLine(); ImGui::Text("To"); ImGui::SameLine(); ImGui::InputInt("##TO",&to);
      ImGui::SameLine(); ImGui::Text("Header"); ImGui::SameLine(); ImGui::InputInt("##HEADER",&header);
      ImGui::PopItemWidth();
      if (from < 0) from = 0;
      if (to < 0) to = 0;
      if (header < 0) header = 0;
      if (from >= (int)begins.size()) from = (int)begins.size() - 1;
      if (to > (int)begins.size()) to = (int)begins.size();
      if (to < from) to = from;
      if (header >= (int)begins.size()) header = (int)begins.size()-1;
      ImGui::Columns(maxLen);
      {for (int i = header; i < header+1; i++) {
        int lh = ends[i] - begins[i];
        for (int j = 0; j < maxLen;j++) {
          if (j < lh) {ImGui::Text("%s",particles[j+begins[i]].c_str()); toolTipHere(particles[j+begins[i]]);}
          else ImGui::TextWrapped("");
          ImGui::NextColumn();
        }
      }}
    ImGui::End();

    String search = toLower(searchText);
    ImGui::SetNextWindowPos(ImVec2(0,headerSize));
    ImGui::SetNextWindowSize(ImVec2(glFrameBufferWidth,glFrameBufferHeight-headerSize));
    ImGui::Begin("CSV",NULL,ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse);
      if (scrollAdd != 0) {
        ImGui::SetScrollY(ImGui::GetScrollY()+scrollAdd);
      }
      ImGui::Columns(maxLen);
      {for (int i = from; i < to; i++) {
        bool valid = true;
        if (!search.empty()) {
          valid = false;
          for (int k = begins[i]; k < ends[i]; k++) {
            if (toLower(particles[k]).find(search)>=0) {valid = true; break;}
          }    
        }
        if (!valid) continue;
        ImGui::Separator();
        int lh = ends[i] - begins[i];
        for (int j = 0; j < maxLen;j++) {
          if (j < lh) {ImGui::TextWrapped("%s",particles[j+begins[i]].c_str()); toolTipHere(particles[j+begins[i]]);}
          else ImGui::TextWrapped("");
          ImGui::NextColumn();
        }
      }}
    ImGui::End();

    ImGui::Render();
      
    glRefreshMasked();
  }

  glDone();
  return 0;
}
