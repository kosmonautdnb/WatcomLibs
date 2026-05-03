#include "DOS.HPP"
#include "STRING.HPP"
#include <conio.h>

char longFileName[MAX_LFN_PATH];

int main(int argc, const char *argv[]) {

  String filterForExtension = "";
  if (argc == 2) filterForExtension = "."+toLower(String(argv[1]));

  String currentPath = getCWD();
  Array<File> filesHere = dosGetFiles(dosFileName(currentPath));
  while(!filesHere.empty()) {
    File here = filesHere[0]; filesHere.erase(0,1);
    if (here.name.name == ".") continue;
    if (here.name.name == "..") continue;

    bool isDir = (here.attributes & DOS_ATTR_SUBDIR);
    if ((!isDir) && (!filterForExtension.empty())) if (toLower(here.name.extension)!=filterForExtension) continue;

    sprintf(longFileName,here.name.full.c_str());
    doslfnLongPath(longFileName, longFileName);

    printf("%s %s%s : %s\n", isDir ? "DIR" : "   ", here.name.name.c_str(), here.name.extension.c_str(), longFileName);
    if (isDir) {
      Array<File> files2 = dosGetFiles(dosFileName(here.name.full+"/"));
      for (int i = 0; i < (int)files2.size(); i++) {
        File here2 = files2[i]; filesHere.insert(here2,i);
      }
    }
    if (kbhit()) break;
  }
  return 0;
}
