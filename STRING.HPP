#ifndef __STRING_HPP__                  
#define __STRING_HPP__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "types.hpp"
#include "object.hpp"

class String : public Object {
public:
  char *data;
  int32_t dataLength;
  
  String() {
    data = NULL;
    dataLength = 0;
  }

  const String &operator=(const String &b) {
    if (&b == this) return *this;
    if (b.data != NULL) {
            if (data != NULL) {
              delete[] data;
            }
            dataLength = b.dataLength;
            data = new char[dataLength];
            memcpy(data,b.data,dataLength);
    } else {
            data = NULL;
    }
    return *this;
  }
  
  bool operator==(const String &b) const {
    if (b.length() != length())
      return false;
    for (int32_t i = 0; i < length(); i++) {
      if (b[i] != (*this)[i])
              return false;
    }
    return true;
  }
  
  bool operator!=(const String &b) const {
    return !(*this == b);
  }
  
  bool operator<(const String &b) const {
    int32_t l = length();
    if (b.length()<l) l = b.length();
    for (int32_t i = 0; i < l; i++) {
      if (b[i]==(*this)[i]) continue;
      return b[i]<(*this)[i];
    }
    return b.length()<length();
  }
  
  bool operator>(const String &b) const {
    int32_t l = length();
    if (b.length()<l) l = b.length();
    for (int32_t i = 0; i < l; i++) {
      if (b[i]==(*this)[i]) continue;
      return b[i]>(*this)[i];
    }
    return b.length()>length();
  }
  
  const String &operator+=(const String &b) {
    add(b);
    return *this;
  }

  const String &operator+=(const Object &b) {
    add(::toString(b));
    return *this;
  }
  
  const String &operator+=(const char *b) {
    add(String(b));
    return *this;
  }
  
  const String &operator+=(int32_t b) {
    add(String(b));
    return *this;
  }
  
  String(const String &b) {
    if (b.data != NULL)
    {
      dataLength = b.dataLength;
      data = new char[dataLength];
      memcpy(data,b.data,dataLength);
    } else {
      data = NULL;
      dataLength = 0;
    }
  }

  String(const Object &b) {
    String c = ::toString(b);
    if (c.data != NULL)
    {
      dataLength = c.dataLength;
      data = new char[dataLength];
      memcpy(data,c.data,dataLength);
    } else {
      data = NULL;
      dataLength = 0;
    }
  }
  
  String(const char *_data) {
    if (_data != NULL)
    {
      dataLength = strlen(_data)+1;
      data = new  char[dataLength];
      memcpy(data,_data,dataLength);
    } else {
      data = NULL;
      dataLength = 0;
    }
  }

  String(const char *_data, int _dataLength) {
    if (_data != NULL)
    {
      dataLength = _dataLength;
      data = new  char[dataLength];
      memcpy(data,_data,dataLength);
    } else {
      data = NULL;
      dataLength = 0;
    }
  }
  
  String(const int32_t v) {
    String b;
    b.resize(32);
    uint32_t va = v;
    if (v < 0) va = -v;
    int32_t i = b.length();
    do {
      i--;
      b[i]=(char)((va % 10)+'0');
      va /= 10;
    } while(va>0);
    if (v < 0) {i--;b[i]='-';}
    dataLength = b.dataLength-i;
    data = new char[dataLength];
    memcpy(data,b.data+i,dataLength);
  }
  
  virtual ~String() {
    if (data != NULL) delete[] data;
  }

  virtual uint32_t hash() const;
  virtual bool equals(Object *a) const;
  virtual String toString() const;

  void clear() {
    if (data != NULL) delete[] data;
    data = NULL;
    dataLength = 0;
  }
  
  void resize(int32_t newLength) {
    if (data == NULL) {
      dataLength = newLength + 1;
      data = new char[dataLength];
      data[dataLength-1] = 0;
    } else {
      if (newLength < dataLength) {
        data[newLength] = 0;
        dataLength = newLength + 1;
      } else {
        char *data2 = new char[newLength + 1];
        memcpy(data2,data,dataLength);
        dataLength = newLength + 1;
        data2[dataLength - 1] = 0;
        delete[] data;
        data = data2;
      }
    }
  }
  
  int32_t length() const {
    if (data == NULL) return 0;
    return dataLength - 1;
  }

  int32_t size() const {
    return length();
  }
  
  char &operator[](int32_t index) const {
    return data[index];
  }
  
  bool empty() const {
    return length()==0;
  }

  static String asHex(int32_t l, int32_t len) {
    const char *hex="0123456789ABCDEF";
    String k; k.resize(len);
    for (int32_t i = 0; i < len; i++) {
      k[len-1-i] = hex[l & 15];
      l>>=4;
    }
    return k;
  }

  int32_t hexToInt() {
    if (empty()) return 0;
    int32_t number = 0;
    bool sign = false;
    for(int32_t i = 0; i < length(); i++) {
      char c = (*this)[i];
      if (c>='0'&&c<='9') {
        number *= 16;
        number += c - '0';
      }
      if (c>='a'&&c<='f') {
        number *= 16;
        number += c - 'a' + 10;
      }
      if (c>='A'&&c<='F') {
        number *= 16;
        number += c - 'A' + 10;
      }
      if (number==0&&c=='-') sign=true;
    }
    if (sign) number=-number;
    return number;
  }

  static String fromInt(int32_t v) {
    return String(v);
  }

  int32_t toInt() {
    if (empty()) return 0;
    int32_t number = 0;
    bool sign = false;
    for(int32_t i = 0; i < length(); i++) {
      char c = (*this)[i];
      if (c>='0'&&c<='9') {
        number *= 10;
        number += c - '0';
      }
      if (number==0&&c=='-') sign=true;
    }
    if (sign) number=-number;
    return number;
  }

  static String fromChar(const char c) {
    String k = " ";
    k[0] = c;
    return k;
  }

  char toChar() {
    if (empty()) return 0;
    return (*this)[0];
  }
  
  static String fromFloat(float v) {
    String k;
    k.resize(16);
    //snprintf_x4(&(k[0]),16,"%f",v);
    sprintf(&(k[0]),"%f",v);
    k.resize(strlen(k.c_str()));
    return k;
  }

  static String fromDouble(double v) {
    String k;
    k.resize(32);
    //snprintf_x4(&(k[0]),32,"%f",v);
    sprintf(&(k[0]),"%f",v);
    k.resize(strlen(k.c_str()));
    return k;
  }

  float toFloat() {
    return (float)atof(c_str());
  }  

  double toDouble() {
    return (double)atof(c_str());
  }  

  char *c_str() const {
    return data;
  }
  
  void erase(int32_t pos, int32_t count) {
    memcpy(data+pos,data+pos+count,length()-(pos+count));
    dataLength -= count;
  }
  
  void insert(int32_t pos, const String &str) {
    int32_t l = length();
    dataLength = l+str.length()+1;
    char *newData = new char[dataLength];
    if (pos>0)  memcpy(newData,data,pos);
    memcpy(newData+pos,str.data,str.length());
    memcpy(newData+pos+str.length(),data+pos,l-pos);
    newData[dataLength-1] = 0;
    if (data != NULL) delete[] data;
    data = newData;
  }
  
  void insert(int32_t pos, const char c) {
    insert(pos,String::fromChar(c));
  }
  
  String substr(int32_t firstIndex) const {
    if (firstIndex == -1) return "";
    if (firstIndex >= length()) return "";// String(*this);
    String r;
    r.resize(length()-firstIndex);
    memcpy(&(r[0]),&((*this)[firstIndex]),r.length());
    return r;
  }
  
  String old_substr(int32_t firstIndex, int32_t lastIndex) const {
    if (lastIndex == -1) return substr(firstIndex);
    if (firstIndex == -1) return "";
    if (firstIndex >= length()) return String(*this);
    String r;
    r.resize(lastIndex-firstIndex);
    memcpy(&(r[0]),&((*this)[firstIndex]),r.length());
    return r;
  }

  String substr(int32_t firstIndex, int32_t count) const {
    // more like std::string.substr() now, sorry. (old function now in old_substr())
    if (count < 0) return substr(firstIndex);
    return old_substr(firstIndex, firstIndex+count);
  }
  
  int32_t findLast(const String &v) const {
    int32_t l1 = length();
    int32_t l2 = v.length();
    for (int32_t i = l1 - l2; i >= 0; i--) {
      bool found = true;
      for (int32_t j = 0; j < l2; j++) {
        if ((*this)[i+j]!=v[j]) {
          found = false;
          break;
        }
      }
      if (found) return i;
    }
    return -1;
  }

  int32_t findFirst(const String &v) const {
    int32_t l1 = length();
    int32_t l2 = v.length();
    for (int32_t i = 0; i <= l1-l2; i++) {
      bool found = true;
      for (int32_t j = 0; j < l2; j++) {
        if ((*this)[i+j]!=v[j]) {
          found = false;
          break;
        }
      }
      if (found) return i;
    }
    return -1;
  }

  int32_t findFirst(int32_t p, const String &v) const {
    int32_t l1 = length();
    int32_t l2 = v.length();
    for (int32_t i = p; i <= l1-l2; i++) {
      bool found = true;
      for (int32_t j = 0; j < l2; j++) {
        if ((*this)[i+j]!=v[j]) {
          found = false;
          break;
        }
      }
      if (found) return i;
    }
    return -1;
  }

  int32_t find(const String &v) {return findFirst(v);}
  bool startsWith(const String &v) {
    return find(v) == 0;
  }

  bool endsWith(const String &v) {
    return findLast(v) == length()-v.length();
  }
 
  int32_t findLast(int32_t pos, const String &v) const {
    int32_t l1 = pos;
    int32_t l2 = v.length();
    for (int32_t i = l1 - l2; i >= 0; i--) {
      bool found = true;
      for (int32_t j = 0; j < l2; j++) {
        if ((*this)[i+j]!=v[j]) {
          found = false;
          break;
        }
      }
      if (found) return i;
    }
    return -1;
  }
  
  String last(int32_t letters) const {
    if (length()<letters)
      return String(*this);
    return substr(length()-letters);
  }
  
  static String add(const String &a, const String &b);
  void add(const String &b) {
    if (b.data == NULL) return;
    int32_t l = length();
    resize(l+b.length());
    memcpy(data+l,b.data,b.length()+1);
  }
};

String operator+(const String &a, const String &b);
String operator+(const Object &a, const String &b);
String operator+(const String &a, const Object &b);
String operator+(const char *a, const String &b);
String operator+(const String &a, const char *b);
String operator+(const int32_t a, const String &b);
String operator+(const String &a, const int32_t b);
String toLower(const String &v);
#endif //__STRING_HPP__