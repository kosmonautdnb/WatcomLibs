#ifndef __DOS_HPP__
#define __DOS_HPP__

#include "String.hpp"
#include "Array.hpp"

  // get current working directory
String getCWD();
  // disable dos "control c" program break (awful with copy/paste)
void disableControlBreak();
  // read real time clock(rtc) (with register nr)
int rtcRead(int reg);
String currentTimeString();

typedef long FileSize;

class FileTime {
public:
  int day;
  int month;
  int year;
  int hour;
  int minute;
  int second;
  int millisecond;
};

bool operator<(const FileTime &a, const FileTime &b);

class FileName {
public:
  String full;
  String name;
  String extension;
};

#define DOS_ATTR_NORMAL 0x00  // Normal file - read/write permitted
#define DOS_ATTR_RDONLY 0x01   // Read-only file
#define DOS_ATTR_HIDDEN 0x02   // Hidden file
#define DOS_ATTR_SYSTEM 0x04 // System file
#define DOS_ATTR_VOLID 0x08    // Volume-ID entry
#define DOS_ATTR_SUBDIR 0x10   // Subdirectory
#define DOS_ATTR_ARCH 0x20    // Archive file

class File {
public:
  FileName name;
  FileTime date;
  FileSize size;
  int attributes;
};

Array<File> dosGetFiles(const String &filePath);
FileTime dosGetFileTime(const String &filePath);
bool dosHasDrive(const char driveLetter);

void dosCreateFolder(const String &folderName);
void dosDeleteFolder(const String &folderName);
void dosRemoveFile(const String &fileName);
void dosRenameFile(const String &oldFileName, const String &newFileName);

String unixFileName(const String &filePath);
String dosFileName(const String &filePath);

String getPath(const String &fileName);
String getName(const String &fileName);
String getExtension(const String &fileName);

#endif //__DOS_HPP__