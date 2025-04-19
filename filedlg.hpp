#ifndef __FILEDLG_HPP
#define __FILEDLG_HPP

#include "array.hpp"
#include "dos.hpp"

class FileDialog {
public:
  bool doDisplay;
  bool doLoad;
  String currentPath;
  int currentFileFormatsItem;
  const char *items;
  Array<File> files;
  enum {
    SORT_DATE,
    SORT_NAME,
    SORT_EXT,
    SORT_ATTR,
  };
  int sortType;
  int sortOrder;
  String currentSelectedFile;
  char folderText[100];
  char saveFileName[100];
  Array<char> driveLetters;

  FileDialog();

  void open(bool loadInsteadOfSave, const char *items = "*.*\0\0"); //\0\0 is terminator, \0 is next section
  bool display(); // mainloop function returns true if finished
  String getSelectedFileName(); // the fileName to load save is returned here
  void close();

  void init();
  void checkDriveLetters();
  void syncFileData();
  void oneFolderUp();
  void filter();
  void sort();
  String dateString(const FileTime &a);
  String attributeString(int a);
};

#endif // __FILEDLG_HPP
