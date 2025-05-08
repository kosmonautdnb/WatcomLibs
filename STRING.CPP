#include "string.hpp"

int32_t String::hash() const {
  int32_t hash = 0;
  for (int32_t i = 0; i < length(); i++) {
    hash += (*this)[i]*((i%547)+1);
  }
  hash += (int32_t)length();
  return hash;
}

bool String::equals(Object *a) const {
  String *v = dynamic_cast<String*>(a);
  if (v == NULL) return false;
  return *v == *this;
}

String String::toString() const {
  return *this;
}

String String::add(const String &a, const String &b) {
        if (a.data == NULL) return b.data == NULL ? String("") : String(b);
        if (b.data == NULL) return String(a);
        String r;
        r.resize(a.length()+b.length());
        memcpy(r.data,a.data,a.length());
        memcpy(r.data+a.length(),b.data,b.length()+1);
        return r;
}

String operator+(const String &a, const String &b) {
        return String::add(a,b);
}

String operator+(const Object &a, const String &b) {
        return String(a) + b;
}

String operator+(const String &a, const Object &b) {
        return a + String(b);
}

String operator+(const char *a, const String &b) {
        return String(a) + b;
}

String operator+(const String &a, const char *b) {
        return a + String(b);
}

String operator+(const int64_t a, const String &b) {
        return String(a) + b;
}

String operator+(const String &a, const int64_t b) {
        return a + String(b);
}

String toLower(const String &v) {
  String r = v;
  for (int64_t i = 0; i < r.length(); i++) {
    if (r[i]>='A'&&r[i]<='Z') r[i]=r[i]-'A'+'a';
  }
  return r;
}