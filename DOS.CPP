#include "types.hpp"
#include <direct.h>
#include "dos.hpp"
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <conio.h>
#ifndef NOT_DOS
#include <i86.h>
#endif
#ifdef ON_WINDOWS
#include <vector>
#include <string>
#include <windows.h>
#include <fileapi.h>
#include <filesystem>
#include <sys/stat.h>
#endif
#include <fcntl.h>

String getCWD() {
#ifndef NOT_DOS
    String currentWorkingDirectory;
  currentWorkingDirectory.resize(2048);
  getcwd(&(currentWorkingDirectory[0]),currentWorkingDirectory.length());
  currentWorkingDirectory.resize(strlen(currentWorkingDirectory.c_str()));
  return unixFileName(currentWorkingDirectory+"/");
#endif
#ifdef ON_WINDOWS
 return unixFileName(String((std::filesystem::current_path().string().c_str())))+"/";
#endif
}

void disableControlBreak() {
#ifndef NOT_DOS
  union REGS regs;
  regs.h.ah = 0x33;
  regs.h.al = 1;
  regs.h.dl = 0;
  int386(0x21, &regs, &regs);
#endif
}

int rtcRead(int reg) {
#ifndef NOT_DOS
  int volatile v=0;
  for (int j=1; j < 100; j++) {v++;}
  outp(0x70,reg & 0xff);
  for (int i=1; i < 100; i++) {v++;}
  return inp(0x71) & 0xff;
#else
  return 0;
#endif
}

static String hex(int v) {
  const char *h="0123456789ABCDEF";
  String r;
  r.resize(2);
  r[0] = h[v / 16];
  r[1] = h[v & 15];
  return r;
}

String currentTimeString() {
  int rtc_seconds = rtcRead(0);
  int rtc_minutes = rtcRead(2);
  int rtc_hours   = rtcRead(4);
  int rtc_weekday = rtcRead(6);
  int rtc_day     = rtcRead(7);
  int rtc_month   = rtcRead(8);
  int rtc_year    = rtcRead(9);
  return hex(rtc_day)+"."+hex(rtc_month)+"."+hex(rtc_year)+" "+hex(rtc_hours)+":"+hex(rtc_minutes)+"."+hex(rtc_seconds);
}

static short _dosGetFileDate(const String &filePath) {
#ifndef NOT_DOS
  int handle = open(filePath.c_str(),O_RDONLY | O_TEXT);
  union REGS r;
  r.h.ah = 0x57;
  r.h.al = 0x00;
  r.w.bx = (short)handle;
  int386(0x21,&r,&r);
  close(handle);
  return r.w.dx;
#else
    return 0;
#endif
}

static short _dosGetFileTime(const String &filePath) {
#ifndef NOT_DOS
    int handle = open(filePath.c_str(),O_RDONLY | O_TEXT);
  union REGS r;
  r.h.ah = 0x57;
  r.h.al = 0x00;
  r.w.bx = (short)handle;
  int386(0x21,&r,&r);
  close(handle);
  return r.w.cx;
#else
    return 0;
#endif
}

static FileTime toFileTime(int date, int time) {
  FileTime r;
  r.day = date & 31;
  r.month = (date >> 5) & 15;
  r.year = (date>>5>>4)-20+2000;
  r.minute = (time>>5) & 63;
  r.second = ((time) & 31)*2; // dunno & 31? maybe * 2
  if (r.second>59) r.second = 59; // not sure about that
  r.hour = (time>>5>>6);
  return r;
}

static FileName toFileName(const String &path, const char *fileName) {
  FileName r;
  r.extension = getExtension(fileName);
  r.name = getName(fileName);
  if (String(fileName) == "..") {r.name = ".."; r.extension = "";}
  if (String(fileName) == ".") {r.name = "."; r.extension = "";}
  r.full = path + r.name + r.extension;
  return r;
}

bool operator<(const FileTime &a, const FileTime &b) {
  if (a.year < b.year) return true;
  if (a.year == b.year && a.month < b.month) return true;
  if (a.year == b.year && a.month == b.month && a.day < b.day) return true;
  if (a.year == b.year && a.month == b.month && a.day == b.day && a.hour < b.hour) return true;
  if (a.year == b.year && a.month == b.month && a.day == b.day && a.hour == b.hour && a.minute < b.minute) return true;
  if (a.year == b.year && a.month == b.month && a.day == b.day && a.hour == b.hour && a.minute == b.minute && a.second < b.second) return true;
  if (a.year == b.year && a.month == b.month && a.day == b.day && a.hour == b.hour && a.minute == b.minute && a.second == b.second && a.millisecond < b.millisecond) return true;
  return false;
}

Array<File> dosGetFiles(const String &filePath) {
#ifndef NOT_DOS
  Array<File> ret;
  DIR *dirp;
  struct dirent *direntp;
  dirp = opendir( dosFileName(filePath).c_str() );
  if( dirp != NULL ) {
    do {
      direntp = readdir( dirp );
      if( direntp == NULL )
        break;
      File r;
      r.name = toFileName(filePath,direntp->d_name);
      r.size = direntp->d_size;
      r.date = toFileTime(direntp->d_date,direntp->d_time);
      r.attributes = direntp->d_attr;
      ret.push_back(r);
    } while(1==1);
    closedir( dirp );
  }
  return ret;
#endif
#ifdef ON_WINDOWS
  Array<File> ret;
  std::string path = filePath.c_str();
  FileTime fileTime;
  fileTime.day = 0;
  fileTime.hour = 0;
  fileTime.millisecond = 0;
  fileTime.minute = 0;
  fileTime.month = 0;
  fileTime.second = 0;
  fileTime.year = 0;
  File r;
  r.date = fileTime;
  r.name.full = "..";
  r.name.name = "..";
  r.name.extension = "";
  r.attributes = DOS_ATTR_SUBDIR;
  ret.push_back(r);
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    File r;
    r.name = toFileName(filePath, entry.path().filename().string().c_str());
    r.size = entry.file_size();
    FileTime fileTime = dosGetFileTime(dosFileName(r.name.full));
    r.date = fileTime;
    r.attributes = (entry.is_directory() ? DOS_ATTR_SUBDIR : 0);
    ret.push_back(r);
  }
  return ret;
#endif
}

FileTime dosGetFileTime(const String &filePath) {
#ifndef NOT_DOS
  FileTime r;
  short date = _dosGetFileDate(filePath);
  short time = _dosGetFileTime(filePath);
  r = toFileTime(date,time);
  return r;
#endif
#ifdef ON_WINDOWS
  FileTime r;
  struct stat filestat;
  if (stat(filePath.c_str(), &filestat) == 0) {
      struct tm* timeinfo;
      timeinfo = localtime(&filestat.st_mtime);
      if (timeinfo != NULL) {
          r.day = timeinfo->tm_mday;
          r.month = timeinfo->tm_mon + 1;
          r.year = timeinfo->tm_year + 1900;
          r.hour = timeinfo->tm_hour;
          r.minute = timeinfo->tm_min;
          r.second = timeinfo->tm_sec;
          r.millisecond = 0;
      }
  }
  return r;
#endif
}

bool dosHasDrive(const char driveLetter) {
#ifndef NOT_DOS
    int d = -1;
  if (driveLetter >= 'A' && driveLetter <= 'Z') d = driveLetter - 'A' + 1;
  if (driveLetter >= 'a' && driveLetter <= 'z') d = driveLetter - 'a' + 1;
  if (d < 0) return false;

  union REGS r;
  r.h.ah = 0x44;
  r.h.al = 0x08;
  r.h.bl = d & 255;
  int386(0x21,&r,&r);
  return (r.w.ax==0x00)||(r.w.ax==0x01);
#endif
#ifdef ON_WINDOWS
  std::vector<std::string> arrayOfDrives;
  wchar_t* szDrives = new wchar_t[MAX_PATH]();
  if (GetLogicalDriveStringsW(MAX_PATH, szDrives));
  for (int i = 0; i < ('Z'-'A')*4; i += 4) {
    if (szDrives[i] != (char)0) {
    wchar_t q = szDrives[i];
      if (q == (L'A' + (driveLetter - 'A'))) {
        delete[] szDrives;
        return true;
      }
    }
  }
  delete[] szDrives;
  return false;
#endif
}

void dosCreateFolder(const String &folderName) {
  mkdir(dosFileName(folderName).c_str());
}

void dosDeleteFolder(const String &folderName) {
  rmdir(dosFileName(folderName).c_str());
}

void dosRemoveFile(const String &fileName) {
  remove(dosFileName(fileName).c_str());
}

void dosRenameFile(const String &oldFileName, const String &newFileName) {
  rename(dosFileName(oldFileName).c_str(), dosFileName(newFileName).c_str());
}

String unixFileName(const String &filePath) {
  String r = String(filePath);
  for (int i = 0; i < r.length(); i++) if (r[i]=='\\') r[i]='/';
  return r;
}

String dosFileName(const String &filePath) {
  String r = String(filePath);
  for (int i = 0; i < r.length(); i++) if (r[i]=='/') r[i]='\\';
  return r;
}

String getPath(const String &fileName) {
  int64_t k0 = fileName.findLast("/");
  int64_t k1 = fileName.findLast("\\");
  int64_t k = fileName.length();
  if (k0 < k1) k0 = k1;
  if (k0>0) k = k0+1;
  return fileName.substr(0,k);
}

String getName(const String &fileName) {
  int64_t k0 = fileName.findLast("/");
  int64_t k1 = fileName.findLast("\\");
  int64_t k = 0;
  if (k0 < k1) k0 = k1;
  if (k0>0) k = k0+1;
  return fileName.substr(k,fileName.findLast("."));
}

String getExtension(const String &fileName) {
  return fileName.substr(fileName.findLast("."));
}

#ifndef NOT_DOS // {--

// long file name extensions (maybe you need doslfn.com)
//---------
int dosErrorCode = 0;
int dosErrorAdditional = 0;

// int 21h long file name functions (not all implemented here)
#define LFN_RESETDRIVE 0x710d
#define LFN_CREATEDIRECTORY 0x7139
#define LFN_REMOVEDIRECTORY 0x713A
#define LFN_SETCURRENTDIRECTORY 0x713B
#define LFN_DELETEFILE 0x7141
#define LFN_FILEATTRIBS 0x7143
#define LFN_GETCURRENTDIRECTORY 0x7147
#define LFN_FINDFIRSTFILE 0x714e
#define LFN_FINDNEXTFILE 0x714f
#define LFN_MOVERENAME 0x7156
#define LFN_TRUENAME 0x7160
#define LFN_EXTENDEDOPEN 0x716C
#define LFN_GETVOLUMEINFO 0x71a0
#define LFN_FINDCLOSE 0x71a1
#define LFN_GETFILEINFO 0x71a6
#define LFN_TIMECONVERSION 0x71a7
#define LFN_GENERATESHORTNAME 0x71a8
#define LFN_SERVERCREATEOPEN 0x71a9
#define LFN_CREATETERMINATE 0x71aa

bool basicDosCallWithString(uint16_t functionNr, const char *str, bool si = false, uint16_t bx = 0, REGS16 *result = NULL, uint16_t dx = 0) {
  DOSPOINTER dosInterfaceMemory = dosMalloc(strlen(str)+1);
  if (dosInterfaceMemory.ptr == NULL) return false;
  sprintf((char*)dosInterfaceMemory.ptr,"%s",str);
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax = functionNr;
  regs.bx = bx;
  regs.dx = dx;
  regs.si = si ? 1 : 0;
  regs.flags = 0;
  regs.ds = dosInterfaceMemory.segment;
  dosInt386x(0x21, &regs, &regs);
  dosFree(dosInterfaceMemory);
  if (regs.flags & 1) {
    dosErrorCode = regs.ax;
    dosErrorAdditional = 0;
    return false;
  }
  if (result != NULL) *result = regs;
  return true;
}

bool doslfnCreateDirectory(const char *name) {
  return basicDosCallWithString(0x7139, name);
}

bool doslfnDeleteDirectory(const char *name) {
  return basicDosCallWithString(0x713a, name);
}

bool doslfnChangeDirectory(const char *name) {
  return basicDosCallWithString(0x713b, name);
}

bool doslfnDeleteFiles(const char *name, bool useWildCards) {
  return basicDosCallWithString(0x7141, name, useWildCards);
}

FileTime doslfnGetFileTime(const char *name) {
  REGS16 regs;
  bool r = basicDosCallWithString(0x7143, name,false,4,&regs);
  return r ? toFileTime(regs.di,regs.cx) : toFileTime(0,0);
}

bool doslfnGetCWD(char *name, uint16_t onDrive) {
  DOSPOINTER dosInterfaceMemory = dosMalloc(MAX_LFN_PATH);
  if (dosInterfaceMemory.ptr == NULL) return false;
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax = 0x7147;
  regs.flags = 0;
  regs.dx = onDrive;
  regs.si = 0;
  regs.ds = dosInterfaceMemory.segment;
  dosInt386x(0x21, &regs, &regs);
  if (regs.flags & 1) {
    dosFree(dosInterfaceMemory);
    dosErrorCode = regs.ax;
    dosErrorAdditional = 0;
    return false;
  }
  memcpy(name,(char*)dosInterfaceMemory.ptr,strlen((char*)dosInterfaceMemory.ptr)+1);
  dosFree(dosInterfaceMemory);
  return true;
}

bool doslfnRename(const char *oldName, const char *newName) {
  DOSPOINTER dosInterfaceMemory1 = dosMalloc(strlen(oldName)+1);
  if (dosInterfaceMemory1.ptr == NULL) return false;
  DOSPOINTER dosInterfaceMemory2 = dosMalloc(strlen(newName)+1);
  if (dosInterfaceMemory2.ptr == NULL) {
    memcpy((char*)dosInterfaceMemory1.ptr,oldName,strlen(oldName)+1);
    return false;
  }
  memcpy((char*)dosInterfaceMemory1.ptr,oldName,strlen(oldName)+1);
  memcpy((char*)dosInterfaceMemory2.ptr,newName,strlen(newName)+1);
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax = 0x7156;
  regs.ds = dosInterfaceMemory1.segment;
  regs.dx = 0;
  regs.es = dosInterfaceMemory2.segment;
  regs.si = 0;
  regs.flags = 0;
  dosInt386x(0x21, &regs, &regs);
  dosFree(dosInterfaceMemory1);
  dosFree(dosInterfaceMemory2);
  if (regs.flags & 1) {
    dosErrorCode = regs.ax;
    dosErrorAdditional = 0;
    return false;
  }
  return true;
}

bool doslfnPath(char *name, const char *fileName, bool longPath) {
  DOSPOINTER dosInterfaceMemory1 = dosMalloc(strlen(fileName)+1);
  if (dosInterfaceMemory1.ptr == NULL) return false;
  memcpy((char*)dosInterfaceMemory1.ptr,fileName,strlen(fileName)+1);
  DOSPOINTER dosInterfaceMemory2 = dosMalloc(MAX_LFN_PATH);
  if (dosInterfaceMemory2.ptr == NULL) {
    dosFree(dosInterfaceMemory1);
    return false;
  }
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax = 0x7160;
  regs.cx = (longPath ? 2 : 1)+0x100;
  regs.ds = dosInterfaceMemory1.segment;
  regs.si = 0;
  regs.es = dosInterfaceMemory2.segment;
  regs.di = 0;
  regs.flags = 0;
  dosInt386x(0x21, &regs, &regs);
  if (regs.flags & 1) {
    dosFree(dosInterfaceMemory1);
    dosFree(dosInterfaceMemory2);
    dosErrorCode = regs.ax;
    dosErrorAdditional = 0;
    return false;
  }
  memcpy(name,(char*)dosInterfaceMemory2.ptr,strlen((char*)dosInterfaceMemory2.ptr)+1);
  dosFree(dosInterfaceMemory1);
  dosFree(dosInterfaceMemory2);
  return true;
}

bool doslfnLongPath(char *name, const char *fileName) {
  return doslfnPath(name, fileName, true);
}

bool doslfnShortPath(char *name, const char *fileName) {
  return doslfnPath(name, fileName,false);
}

int32_t doslfnOpen(const char *fileName, bool read) {
  DOSPOINTER dosInterfaceMemory = dosMalloc(strlen(fileName)+1);
  if (dosInterfaceMemory.ptr == NULL) return false;
  memcpy((char*)dosInterfaceMemory.ptr,fileName,strlen(fileName)+1);
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax = 0x716c;
#define SHAREMODE(__COMMIT__,__ERRORHANDLING__,__ALIAS__,__COMPRESS__,__BUFFER__,__INHERIT__,__SHARE__,__ACCESS__)\
  ((__COMMIT__)<<14)|\
  ((__ERRORHANDLING__)<<13)|\
  ((__ALIAS__)<<10)|\
  ((__COMPRESS__)<<9)|\
  ((__BUFFER__)<<8)|\
  ((__INHERIT__)<<7)|\
  ((__SHARE__)<<4)|\
  ((__ACCESS__)<<0)
  regs.bx = SHAREMODE(1,0,0,0,0,0,read?2:3,read?0:1);
  regs.cx = 0; // File Attributes
  regs.dx = ((read ? 0 : 1)<<4)|(read?1:2);
  regs.ds = dosInterfaceMemory.segment;
  regs.si = 0;
  regs.flags = 0;
  dosInt386x(0x21, &regs, &regs);
  dosFree(dosInterfaceMemory);
  if (regs.flags & 1) {
    dosErrorCode = regs.ax;
    dosErrorAdditional = 0;
    return -1;
  }
  int fileHandle = regs.ax;
  return fileHandle;
}

#define FILEIO_BYTES 8192

bool doslfnRead(int32_t fileHandle, void *dest, uint64_t byteCount) {
  DOSPOINTER dosInterfaceMemory = dosMalloc(FILEIO_BYTES);
  if (dosInterfaceMemory.ptr == NULL) return false;

  while(byteCount>0) {
    REGS16 regs;
    memset(&regs,0,sizeof(regs));
    regs.ax = 0x3f00;
    regs.bx = fileHandle & 0xffff;
    uint32_t readCount = byteCount > FILEIO_BYTES ? FILEIO_BYTES : byteCount;
    regs.cx = readCount;
    regs.ds = dosInterfaceMemory.segment;
    regs.dx = 0;
    regs.flags = 0;
    dosInt386x(0x21, &regs, &regs);
    if (regs.flags & 1) {
      dosFree(dosInterfaceMemory);
      dosErrorCode = regs.ax;
      dosErrorAdditional = 0;
      return false;
    }
    memcpy(dest,dosInterfaceMemory.ptr,regs.ax);
    if (regs.ax != readCount) {
      dosFree(dosInterfaceMemory);
      dosErrorCode = 0x00;
      dosErrorAdditional = readCount-regs.ax;
      return false;
    }
    byteCount -= readCount;
    dest = (void*)(((unsigned char*)dest)+readCount);
  }
  dosFree(dosInterfaceMemory);
  return true;
}

bool doslfnWrite(int32_t fileHandle, void *source, uint64_t byteCount) {
  DOSPOINTER dosInterfaceMemory = dosMalloc(FILEIO_BYTES);
  if (dosInterfaceMemory.ptr == NULL) return false;

  do {
    REGS16 regs;
    memset(&regs,0,sizeof(regs));
    regs.ax = 0x4000;
    regs.bx = fileHandle & 0xffff;
    uint32_t writeCount = byteCount > FILEIO_BYTES ? FILEIO_BYTES : byteCount;
    regs.cx = writeCount;
    memcpy(dosInterfaceMemory.ptr,source,writeCount);
    regs.ds = dosInterfaceMemory.segment;
    regs.dx = 0;
    regs.flags = 0;
    dosInt386x(0x21, &regs, &regs);
    if (regs.flags & 1) {
      dosFree(dosInterfaceMemory);
      dosErrorCode = regs.ax;
      dosErrorAdditional = 0;
      return false;
    }
    if (regs.ax != writeCount) {
      dosFree(dosInterfaceMemory);
      dosErrorCode = 0x00;
      dosErrorAdditional = writeCount-regs.ax;
      return false;
    }
    byteCount -= writeCount;
    source = (void*)(((unsigned char*)source)+writeCount);
  } while(byteCount>0);
  dosFree(dosInterfaceMemory);
  return true;
}

bool doslfnSeek(int32_t fileHandle, uint64_t pos, uint32_t seekMode) {
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax = 0x4200+(seekMode==SEEK_SET?0:0)+(seekMode==SEEK_CUR?1:0)+(seekMode==SEEK_END?2:0);
  regs.bx = fileHandle & 0xffff;
  regs.cx = (pos >> 16) & 0xffff;
  regs.dx = pos & 0xffff;
  regs.flags = 0;
  dosInt386x(0x21, &regs, &regs);
  if (regs.flags & 1) {
    return false;
  }
  return true;
}

bool doslfnClose(int32_t fileHandle) {
  REGS16 regs;
  memset(&regs,0,sizeof(regs));
  regs.ax  = 0x3e00;
  regs.bx = fileHandle & 0xffff;
  regs.flags = 0;
  dosInt386x(0x21, &regs, &regs);
  if (regs.flags & 1) {
    dosErrorCode = regs.ax;
    dosErrorAdditional = 0;
    return false;
  }
  return true;
}

uint64_t doslfnTell(int32_t fileHandle) {
  FILE k;
  memset(&k,0,sizeof(FILE));
  k._handle = fileHandle;
  return ftell(&k);
}

uint64_t doslfnSize(int32_t fileHandle) {
  uint64_t cp = doslfnTell(fileHandle);
  doslfnSeek(fileHandle,0,SEEK_END);
  uint64_t r = doslfnTell(fileHandle);
  doslfnSeek(fileHandle,cp,SEEK_SET);
  return r;
}

// lowlevel DPMI and DOS calls
//---------
__inline DOSPOINTER dosNullPointer() {
  DOSPOINTER k;
  k.selector = 0;
  k.segment = 0;
  k.ptr = NULL;
  return k;
}

__inline DOSPOINTER makeDosPointer(uint16_t selector, uint16_t segment) {
  DOSPOINTER k;
  k.selector = selector;
  k.segment = segment;
  k.ptr = (void*)((unsigned int)segment * 16);
  return k;
}

DOSPOINTER dosMalloc(uint32_t size) {
  union REGS regs;
  regs.w.ax = 0x100;
  regs.w.bx = (size+15)/16;
  regs.w.cflag = false;
  int386(0x31, &regs, &regs);
  if (regs.w.cflag) {
    dosErrorCode = regs.w.ax;
    dosErrorAdditional = (unsigned int)regs.w.bx*16; // largest allocatable block
    return dosNullPointer();
  }
  return makeDosPointer(regs.w.dx,regs.w.ax);
}

bool dosFree(const DOSPOINTER &mem) {
  union REGS regs;
  regs.w.ax = 0x101;
  regs.w.dx = mem.selector;
  regs.w.cflag = false;
  int386(0x31, &regs, &regs);
  if (regs.w.cflag) {
    dosErrorCode = regs.w.ax;
    dosErrorAdditional = 0;
    return false;
  }
  return true;
}

typedef struct {
  long edi;
  long esi;
  long ebp;
  long reserved;
  long ebx;
  long edx;
  long ecx;
  long eax;
  short flags;
  short es,ds,fs,gs,ip,cs,sp,ss;
} _REGS16;

uint16_t dosInt386x(uint32_t intno, REGS16 *in, REGS16 *out) {
  _REGS16 rmregs;
  union REGS r;
  struct SREGS sr;
  segread(&sr);
  sr.es = sr.ds; // flat model?
  memset(&rmregs,0,sizeof(rmregs));
  rmregs.eax = in->ax; 
  rmregs.ebx = in->bx; 
  rmregs.ecx = in->cx; 
  rmregs.edx = in->dx; 
  rmregs.esi = in->si; 
  rmregs.edi = in->di; 
  rmregs.ds = in->ds;
  rmregs.es = in->es;
  rmregs.flags = in->cflag ? 0x01 : 0x00;
  r.w.ax = 0x300;
  r.h.bl = intno;
  r.h.bh = 0;
  r.w.cx = 0;
  r.x.edi = (unsigned)&rmregs;
  int386x(0x31,&r,&r,&sr);
  out->ax = rmregs.eax; 
  out->bx = rmregs.ebx; 
  out->cx = rmregs.ecx; 
  out->dx = rmregs.edx; 
  out->si = rmregs.esi; 
  out->di = rmregs.edi; 
  out->es = rmregs.es;
  out->cs = rmregs.cs;
  out->ss = rmregs.ss;
  out->ds = rmregs.ds;
  out->cflag = rmregs.flags & 0x01;
  return out->ax;
}
#endif // NOT_DOS --}
