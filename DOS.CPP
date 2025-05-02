#include <direct.h>
#include "dos.hpp"
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <conio.h>
#include <i86.h>
#include <fcntl.h>

String getCWD() {
  String currentWorkingDirectory;
  currentWorkingDirectory.resize(2048);
  getcwd(&(currentWorkingDirectory[0]),currentWorkingDirectory.length());
  currentWorkingDirectory.resize(strlen(currentWorkingDirectory.c_str()));
  return unixFileName(currentWorkingDirectory+"/");
}

void disableControlBreak() {
  union REGS regs;
  regs.h.ah = 0x33;
  regs.h.al = 1;
  regs.h.dl = 0;
  int386(0x21, &regs, &regs);
}

int rtcRead(int reg) {
  int volatile v=0;
  for (int j=1; j < 100; j++) {v++;}
  outp(0x70,reg & 0xff);
  for (int i=1; i < 100; i++) {v++;}
  return inp(0x71) & 0xff;
}

static String hex(int v) {
  char *h="0123456789ABCDEF";
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
  int handle = open(filePath.c_str(),O_RDONLY | O_TEXT);
  union REGS r;
  r.h.ah = 0x57;
  r.h.al = 0x00;
  r.w.bx = (short)handle;
  int386(0x21,&r,&r);
  close(handle);
  return r.w.dx;
}

static short _dosGetFileTime(const String &filePath) {
  int handle = open(filePath.c_str(),O_RDONLY | O_TEXT);
  union REGS r;
  r.h.ah = 0x57;
  r.h.al = 0x00;
  r.w.bx = (short)handle;
  int386(0x21,&r,&r);
  close(handle);
  return r.w.cx;
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
}

FileTime dosGetFileTime(const String &filePath) {
  FileTime r;
  short date = _dosGetFileDate(filePath);
  short time = _dosGetFileTime(filePath);
  r = toFileTime(date,time);
  return r;
}

bool dosHasDrive(const char driveLetter) {
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
  int k0 = fileName.findLast("/");
  int k1 = fileName.findLast("\\");
  int k = fileName.length();
  if (k0 < k1) k0 = k1;
  if (k0>0) k = k0+1;
  return fileName.substr(0,k);
}

String getName(const String &fileName) {
  int k0 = fileName.findLast("/");
  int k1 = fileName.findLast("\\");
  int k = 0;
  if (k0 < k1) k0 = k1;
  if (k0>0) k = k0+1;
  return fileName.substr(k,fileName.findLast("."));
}

String getExtension(const String &fileName) {
  return fileName.substr(fileName.findLast("."));
}
