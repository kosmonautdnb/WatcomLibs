#define _CRT_SECURE_NO_WARNINGS

#include "TYPES.HPP"

#ifndef NOT_DOS
#include "imguigl.hpp"
#include "gl.h"
#define FDWIDTH 400
#define FDHEIGHT 434
#define FDXTAB 100
#include "hashmap.hpp"
#define spHashMap HashMap
#define spHas(__s__,__v__) has(__v__)
#else
#include <imgui/imgui.h>
#define FDWIDTH 700
#define FDHEIGHT 434
#define FDXTAB 400
#include <map>
#define spHashMap std::map
#define spHas(__s__,__v__) find(__v__)!=(__s__).end()
#endif

#include "filedlg.hpp"

extern int glFrameBufferWidth;
extern int glFrameBufferHeight;

FileDialog::FileDialog() 
{
  init();
}

void FileDialog::open(bool loadInsteadOfSave, const char *items, const FileCallback &callback) {
  init();
  fileCallback = callback;
  doDisplay = true;
  doLoad = loadInsteadOfSave;
  currentPath = getCWD();
  this->items = items;
  syncFileData();
}
  
void FileDialog::close() {
  doDisplay = false;
}

void FileDialog::setCurrentPath(const String &path) {
  currentPath = path;
  syncFileData();
}

String FileDialog::getCurrentPath() {
  return currentPath;
}

bool FileDialog::display() {
  bool ready = false;
  int i;
  if (!doDisplay) return ready;
  ImGui::Begin(doLoad ? "Load File##File Dialog" : "Save File##File Dialog", NULL, ImVec2(FDWIDTH,FDHEIGHT), -1, ImGuiWindowFlags_NoResize);

  Array<String> pathComponents;
  const char *t = currentPath.c_str();
  String current;
  while(*t!=0x00) {
    const char c = *t;
    if (c=='/') {
      pathComponents.push_back(current);
      current = "";
    } else {
      current += String::fromChar(c);
    }
    t++;
  }

  for (i = 0; i < pathComponents.size(); i++) {
    if (i != 0) ImGui::SameLine();
    if (ImGui::Button((pathComponents[i]+"##pathButtons"+toString(i)).c_str())) {
      String newPath;
      for (int j = 0; j <= i; j++) {
        newPath += pathComponents[j] + "/";
        currentPath = newPath;
        syncFileData();
      }
    }
  }

  ImGui::Separator();

  ImGui::Text("Sort");
  ImGui::SameLine(); if (ImGui::Button("Name")) {sortType = SORT_NAME; sortOrder ^= 1;sort();}
  ImGui::SameLine(); if (ImGui::Button("Date")) {sortType = SORT_DATE; sortOrder ^= 1;sort();}
  ImGui::SameLine(); if (ImGui::Button("Ext")) {sortType = SORT_EXT; sortOrder ^= 1;sort();}
  ImGui::SameLine(); if (ImGui::Button("Attr")) {sortType = SORT_ATTR; sortOrder ^= 1;sort();}
  ImGui::SameLine(); if (ImGui::Button("Size")) {sortType = SORT_SIZE; sortOrder ^= 1;sort();}

  ImGui::SameLine(); ImGui::Text("New");
  ImGui::SameLine(); if (ImGui::Button("Folder:")) {
    dosCreateFolder((currentPath + folderText).c_str());
    syncFileData();
  } 
  ImGui::SameLine(); ImGui::InputText("Folder##FolderText",folderText,100);

  ImGui::Separator();

  ImGui::BeginChild("FileView", ImVec2(FDWIDTH,300));
  for (i = 0; i < files.size(); i++) {
    ImVec2 k = ImGui::GetCursorPos();
    String buttonName = files[i].name.name+files[i].name.extension + "##FileButton" + toString(i);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1,1,1,0.2f));
    bool isDir = (files[i].attributes & DOS_ATTR_SUBDIR);
    ImGui::PushStyleColor(ImGuiCol_Text, isDir ? ImVec4(1,1,0,1) : ImVec4(1,1,1,1));
    if (ImGui::Button(buttonName.c_str())) {
      if (!isDir) {
        String fileName = files[i].name.name + files[i].name.extension;
        strcpy(saveFileName,fileName.c_str());
        currentSelectedFile = currentPath + fileName;
        if (fileCallback!=NULL) fileCallback(dosFileName(currentSelectedFile).c_str());
      }
      else {
        strcpy(saveFileName,"");
        if (files[i].name.name == "..") {
          oneFolderUp(); 
          syncFileData();
        }
        else
        if (files[i].name.name != ".") {
          currentPath += files[i].name.name + files[i].name.extension + "/";
          syncFileData();
        } 
      }        
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    k.x += FDXTAB;
    ImGui::SetCursorPos(k);
    ImGui::Text(("   " + sizeString(files[i].attributes,files[i].size) + "\t" + dateString(files[i].date) + " " + attributeString(files[i].attributes)).c_str());
  }
  ImGui::EndChild();

  for (i = 0; i < driveLetters.size(); i++) {
    if (i != 0) ImGui::SameLine();
    int a = driveLetters[i];
    if (ImGui::Button((String::fromChar(a)+":##DriveLetterButton"+toString(i)).c_str())) {
      currentPath = String::fromChar(a) + ":/";
      currentSelectedFile = "";
      strcpy(saveFileName,"");
      syncFileData();
    }
  }

  ImGui::Separator();

  if (doLoad) {
    ImGui::Text(currentSelectedFile.last(40).c_str());
  } else {
    ImGui::InputText("FileName##SAVEFILENAME",saveFileName,100);
  }

  ImGui::Separator();

  if (ImGui::Button("Cancel")||ImGui::IsKeyDown(ImGui::GetIO().KeyMap[ImGuiKey_Escape])) {
    currentSelectedFile = "";
    close();
    ready = true;
  }
  ImGui::SameLine();
  if (ImGui::Combo("##FileFormats", &currentFileFormatsItem, items, 3)) {
    syncFileData();
  }
  ImGui::SameLine();
  if (ImGui::Button(doLoad ? "Load" : "Save")) {
    if (!doLoad) {
      if (strlen(saveFileName)>0)
        currentSelectedFile = currentPath + saveFileName;
      else
        currentSelectedFile = "";
    }
    close();
    ready = true;
  }

  ImGui::End();
  return ready;
}

String FileDialog::getSelectedFileName() {
  return dosFileName(currentSelectedFile);
}

void FileDialog::init() {
  doDisplay = false;
  doLoad = false;
  currentPath = "";
  currentFileFormatsItem = 0;
  items = "*.*\0\0";
  sortType = SORT_NAME;
  sortOrder = 0;
  currentSelectedFile = "";
  strcpy(folderText,"FOLDER");
  strcpy(saveFileName,"");
  driveLetters.clear();
  fileCallback = NULL;
}

void FileDialog::checkDriveLetters() {
  driveLetters.clear();
  for (int i = 'A'; i <= 'Z'; i++) {
    if (dosHasDrive(i))
      driveLetters.push_back(i);
  }
}

void FileDialog::syncFileData() {
  checkDriveLetters();
  files = dosGetFiles(currentPath);
  filter();
  sort();
}

void FileDialog::oneFolderUp() {
  int32_t i = currentPath.findLast("/");
  i = currentPath.findLast(i,"/"); 
  if (i >= 0) currentPath = currentPath.substr(0,i) + "/";
}

void FileDialog::filter() {
  int32_t i;
  for (i = files.size() - 1; i >= 0; i--) {
    if (files[i].name.name == ".") {files.erase(i,1); break;}
  }
  const char *s = items;
  String current;
  spHashMap<String, bool> set;
  int currentItem = 0;

  while (true) {
    char c = *s;
    if (c == ',') {
      if (currentItem==currentFileFormatsItem) set[toLower(current.substr(current.findLast(".")+1))] = true;
      current = "";
    } else
    if (c == 0x00) {
      if (currentItem==currentFileFormatsItem) set[toLower(current.substr(current.findLast(".")+1))] = true;
      if (*(s+1)==0x00) break;
      current = "";
      currentItem++;
    } else {
      current += String::fromChar(c);
    }
    s++;
  }

  if (set.spHas(set,"*")) 
    return;
  for (i = files.size() - 1; i >= 0; i--) {
    if (files[i].attributes & DOS_ATTR_SUBDIR) continue;
    String here = toLower(files[i].name.extension.substr(1));
    if (set.spHas(set,here)) continue;
    files.erase(i,1);
  }
}
 
void FileDialog::sort() {
  for (int i = 0; i < files.size(); i++) {
    for (int j = i+1; j < files.size(); j++) {
      bool bigger = false;
      String nameHere = (files[i].name.name + files[i].name.extension);
      switch(sortType) {
      case SORT_NAME: bigger = nameHere < (files[j].name.name + files[j].name.extension); break;
      case SORT_EXT: bigger = (files[i].name.extension + files[i].name.name) < (files[j].name.extension + files[j].name.name); break;
      case SORT_DATE: bigger = files[i].date < files[j].date; break;
      case SORT_ATTR: bigger = files[i].attributes < files[j].attributes; break;
      case SORT_SIZE: bigger = files[i].size < files[j].size; break;
      }
      if (sortOrder) bigger = !bigger;
      bool dirHere = files[i].attributes & DOS_ATTR_SUBDIR;
      bool dirThere = files[j].attributes & DOS_ATTR_SUBDIR;
      if (dirHere != dirThere) {
        if (dirHere) bigger = false; else bigger = true;
      }
      if (nameHere == "." || nameHere == "..") bigger = false;
      if (bigger) {
        File t = files[i];
        files[i] = files[j];
        files[j] = t;
      }
    }
  }
}

String FileDialog::dateString(const FileTime &a) {
  return String(a.day) + "." + String(a.month) + "." + String(a.year) + " " + String(a.hour) + ":" + String(a.minute);
}

String FileDialog::sizeString(const int a, const int32_t s) {
  if (a & DOS_ATTR_SUBDIR) return "<DIR>";
  if (s < 1024*1024)
    return String(s)+"b";
  if (s < 1024*1024*1024)
    return String(s/1024)+"K";
  return String(s/1024/1024)+"MB";
}

String FileDialog::attributeString(const int a) {
  String r;
  if (a & DOS_ATTR_SUBDIR) r += "D";
  if (a & DOS_ATTR_RDONLY) r += "R";
  if (a & DOS_ATTR_HIDDEN) r += "H";
  if (a & DOS_ATTR_SYSTEM) r += "S";
  return r;
}
