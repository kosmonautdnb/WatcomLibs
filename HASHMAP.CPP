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

int getHash(const int v) {
  return v;
}
