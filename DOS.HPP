#define _CRT_SECURE_NO_WARNINGS
#ifndef __DOS_HPP__
#define __DOS_HPP__

#include "String.hpp"
#include "Array.hpp"

  // get current working directory
String getCWD();
  // disable dos "control c" program break (awful with copy/paste)
void disableControlBreak();
  // read real time clock(rtc) (with register nr)
int rtcRead(unsigned char reg);
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
bool dosIsDriveReady(const char driveLetter);

void dosCreateFolder(const String &folderName);
void dosDeleteFolder(const String &folderName);
void dosRemoveFile(const String &fileName);
void dosRenameFile(const String &oldFileName, const String &newFileName);

String unixFileName(const String &filePath);
String dosFileName(const String &filePath);

String getPath(const String &fileName);
String getName(const String &fileName);
String getExtension(const String &fileName);

// long file name extensions (maybe you need doslfn.com)
//---------
#define MAX_LFN_PATH 260
typedef struct {
  uint32_t attributes;
  uint32_t creationTime;
  uint32_t accessTime;
  uint32_t modificationTime;
  uint32_t driveId;
  uint32_t fileSize;
  uint32_t linkCount;
  uint32_t fileId;
} LFNFILEINFO;
bool doslfnCreateDirectory(const char *name);
bool doslfnDeleteDirectory(const char *name);
bool doslfnChangeDirectory(const char *name);
bool doslfnDeleteFiles(const char *name, bool useWildCards = false);
FileTime doslfnGetFileTime(const char *name);
bool doslfnGetCWD(char *name, uint16_t onDrive=0); // no drive letters returned
bool doslfnRename(const char *oldName, const char *newName);
bool doslfnLongPath(char *name, const char *fileName);
bool doslfnShortPath(char *name, const char *fileName);
int32_t doslfnOpen(const char *fileName, bool read);
bool doslfnRead(int32_t fileHandle, void *dest, uint32_t byteCount);
bool doslfnWrite(int32_t fileHandle, void *source, uint32_t byteCount);
bool doslfnSeek(int32_t fileHandle, uint32_t pos, uint32_t seekMode);
bool doslfnClose(int32_t fileHandle);
uint32_t doslfnTell(int32_t fileHandle);
uint32_t doslfnSize(int32_t fileHandle);
LFNFILEINFO doslfnInfo(int32_t fileHandle); // maybe not supported: errno = 0x7100 in that case

// lowlevel DPMI and DOS calls
//---------
typedef struct {
  uint16_t selector; // for protected mode
  uint16_t segment; // for 16 bit dos mode (offset is 0)
  void *ptr; // flat mode memory to write to
} DOSPOINTER;

typedef struct {
  uint16_t di;
  uint16_t si;
  uint16_t bp;
  uint16_t reserved;
  uint16_t bx;
  uint16_t dx;
  uint16_t cx;
  uint16_t ax;
  uint16_t es,ds,fs,gs,ip,cs,sp,ss;
  bool cflag;
} REGS16;

DOSPOINTER dosMalloc(uint32_t size);
bool dosFree(const DOSPOINTER &mem);
uint16_t dosInt386x(uint32_t intNumber, REGS16 *in, REGS16 *out);

#endif //__DOS_HPP__