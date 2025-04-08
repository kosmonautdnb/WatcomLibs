#include "hashmap.hpp"
#include "string.hpp"

int getHash(const String &v) {
  int hash = 0;
  for (int i = 0; i < v.length(); i++) {
    hash += v[i]*((i%547)+1);
  }
  hash += v.size();
  return hash;
}

int getHash(const uint64_t &v) {
  return (v & 0xffffffff)^(v>>32);
}

int getHash(const int64_t &v) {
  return (v & 0xffffffff)^(v>>32);
}

int getHash(const unsigned int &v) {
  return v;
}

int getHash(const int &v) {
  return v;
}

int getHash(const unsigned char &v) {
  return v;
}

int getHash(const char &v) {
  return v;
}

int getHash(const unsigned short &v) {
  return v;
}

int getHash(const short &v) {
  return v;
}

int getHash(const float &v) {
  return *((int*)&v);
}

int getHash(const double &v) {
  return (*((int*)&v))^*(((int*)&v)+1);
}
