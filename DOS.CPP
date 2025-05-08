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
