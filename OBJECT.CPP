#include "object.hpp"
#include "string.hpp"

uint32_t getHash(const Object &v) {
  return v.hash();
}

uint32_t getHash(const char *v) {
  int32_t hash = 0;
  const char *s=v;
  int32_t i = 0;
  while (*s != 0x00) {
    hash += *s*((i%547)+1);
    s++;
    i++;
  }
  hash += i;
  return hash;
}

uint32_t getHash(const void *v) {
  return (int32_t)((intptr_t)v);
}

uint32_t getHash(const uint8_t &v) {
  return v;
}

uint32_t getHash(const int8_t &v) {
  return v;
}

uint32_t getHash(const uint16_t &v) {
  return v;
}

uint32_t getHash(const int16_t &v) {
  return v;
}

uint32_t getHash(const uint32_t &v) {
  return v;
}

uint32_t getHash(const int32_t &v) {
  return v;
}

uint32_t getHash(const uint64_t &v) {
  return (v & 0xffffffff)^(v>>32);
}

uint32_t getHash(const int64_t &v) {
  return (v & 0xffffffff)^(v>>32);
}

uint32_t getHash(const float &v) {
  return *((int*)&v);
}

uint32_t getHash(const double &v) {
  return (*((int*)&v))^*(((int*)&v)+1);
}

bool equals(const class Object &a,const class Object &b) {
  return a.equals((Object*)&b);
}

bool equals(const char *a,const char *b) {
  if (a == NULL||b==NULL) {
    return a == b;
  }
  if (strlen(a)!=strlen(b)) return false;
  return strcmp(a,b)==0;
}

bool equals(const void *a,const void *b) {
  return a == b;
}

bool equals(const uint8_t &a,const uint8_t &b) {
  return a == b;
}

bool equals(const int8_t &a,const int8_t &b) {
  return a == b;
}

bool equals(const uint16_t &a,const uint16_t &b) {
  return a == b;
}

bool equals(const int16_t &a,const int16_t &b) {
  return a == b;
}

bool equals(const uint32_t &a,const uint32_t &b) {
  return a == b;
}

bool equals(const int32_t &a,const int32_t &b) {
  return a == b;
}

bool equals(const uint64_t &a, const uint64_t &b) {
  return a == b;
}

bool equals(const int64_t &a, const uint64_t &b) {
  return a == b;
}

bool equals(const float &a,const float &b) {
  return a == b;
}

bool equals(const double &a,const double &b) {
  return a == b;
}

String toString(const class Object &v) {
  return v.toString();
}

String toString(const char *v) {
  return String(v);
}

String toString(const void *v) {
  return "[*"+String::asHex((int)((intptr_t)v),8)+"]";
}

String toString(const uint8_t &v) {
  return String::fromInt(v);
}

String toString(const int8_t &v) {
  return String::fromInt(v);
}

String toString(const uint16_t &v) {
  return String::fromInt(v);
}

String toString(const int16_t &v) {
  return String::fromInt(v);
}

String toString(const uint32_t &v) {
  return String::fromInt(v);
}

String toString(const int32_t &v) {
  return String::fromInt(v);
}

String toString(const uint64_t &v) {
  return String::fromInt(v);
}

String toString(const int64_t &v) {
  return String::fromInt(v);
}

String toString(const float &v) {
  return String::fromFloat(v);
}

String toString(const double &v) {
  return String::fromFloat((float)v);
}
