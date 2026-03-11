#include "ARRAY.HPP"
#include "MINIZ.HPP"
#include "DOS.HPP"

String folder;
mz_zip_archive zip;

#define ZIPBUFFERSIZE (1024*64)

char buffer1[MAX_LFN_PATH];
char buffer2[MAX_LFN_PATH];

String longPath(const String &p) {
  memcpy(buffer1,&(p[0]),p.length());
  memcpy(buffer2,&(p[0]),p.length());
  buffer1[p.length()] = 0;
  buffer2[p.length()] = 0;
  if (!doslfnLongPath(buffer1,buffer2)) return p;
  return String((const char*)buffer1);
}

String shortPath(const String &p) {
  memcpy(buffer1,&(p[0]),p.length());
  memcpy(buffer2,&(p[0]),p.length());
  buffer1[p.length()] = 0;
  buffer2[p.length()] = 0;
  if (!doslfnShortPath(buffer1,buffer2)) return p;
  return String((const char*)buffer1);
}

int main(int argc, const char *argv[]) {

  printf("Long filename recursive zipping\n");
  printf("---------------------------------\n");
  if (argc != 3) {
    printf("usage: app_zip.exe <zipname> <folder>\n");
    exit(0);
  }
  String zipName = argv[1];
  FILE *out = fopen(zipName.c_str(),"wb");
  if (out == NULL) {
    printf("Couldn't open zip for write:<%s>\n", zipName.c_str());
    exit(0);
  }
  fclose(out);

  String folderName = argv[2];
  if (!folderName.endsWith("\\")) folderName += "\\";
  if (folderName[1] != ':') folderName = getCWD() + folderName;
  folder = longPath(folderName);
  printf("Packing contents of folder:<%s>\n",folder.c_str());

  Array<String> recursion;
  recursion.push_back(folder);

  mz_zip_zero_struct(&zip);

  if (!mz_zip_writer_init_file(&zip, zipName.c_str(), ZIPBUFFERSIZE)) {
    printf("Zip create error for <%s>\n",zipName.c_str());
    exit(0);
  }

  while(!recursion.empty()) {
    String f = recursion.back(); recursion.pop_back();
    Array<File> current = dosGetFiles(shortPath(f));

    if (f.startsWith(folder)) f = f.substr(folder.length());
    printf("Subdir:<%s>\n",f.c_str());

    for (int i = 0; i < current.size(); i++) {
      if (current[i].name.name == "." || current[i].name.name == "..") continue;
      String fileName = longPath(current[i].name.full);
      if (current[i].attributes & DOS_ATTR_SUBDIR) {
        recursion.push_back(fileName+"\\");
      } else {
        if (fileName.startsWith(folder)) fileName = fileName.substr(folder.length());
        printf("Adding file:<%s>\n",fileName.c_str());
        if (!mz_zip_writer_add_file(&zip, fileName.c_str(), current[i].name.full.c_str(), "", 0, MZ_BEST_COMPRESSION|MZ_ZIP_FLAG_ASCII_FILENAME)) {
          printf("Error adding file:<%s>\n",fileName.c_str());
        }
      }
    }
  }

  if (!mz_zip_writer_finalize_archive(&zip)) {
    printf("Couldn't finalize archive:<%s>\n",zipName.c_str());
    exit(0);
  }

  if (!mz_zip_writer_end(&zip)) {
    printf("Couldn't end zipping archive:<%s>\n",zipName.c_str());
    exit(0);
  }

  return 0;
}
