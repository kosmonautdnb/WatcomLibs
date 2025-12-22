#ifndef __OBJECT_HPP__
#define __OBJECT_HPP__

#include "types.hpp"

class Object {
public:
  virtual uint32_t hash() const = 0;
  virtual bool equals(Object *a) const = 0;
  virtual class String toString() const = 0;
  virtual bool serialize(class Serializer *ser) {return false;}
};

uint32_t2 getHash(const class Object &v);
uint32_t2 getHash(const char *v);
uint32_t2 getHash(const void *v);
uint32_t2 getHash(const uint8_t &v);
uint32_t2 getHash(const int8_t &v);
uint32_t2 getHash(const uint16_t &v);
uint32_t2 getHash(const int16_t &v);
uint32_t2 getHash(const uint32_t2 &v);
uint32_t2 getHash(const int32_t2 &v);
uint32_t2 getHash(const uint64_t &v);
uint32_t2 getHash(const int64_t &v);
uint32_t2 getHash(const float &v);
uint32_t2 getHash(const double &v);

bool equals(const class Object &a,const class Object &b);
bool equals(const char *a,const char *b);
bool equals(const void *a,const void *b);
bool equals(const uint8_t &a,const uint8_t &b);
bool equals(const int8_t &a,const int8_t &b);
bool equals(const uint16_t &a,const uint16_t &b);
bool equals(const int16_t &a,const int16_t &b);
bool equals(const uint32_t2 &a,const uint32_t2 &b);
bool equals(const int32_t2 &a,const int32_t2 &b);
bool equals(const uint64_t &a, const uint64_t &b);
bool equals(const int64_t &a, const uint64_t &b);
bool equals(const float &a,const float &b);
bool equals(const double &a,const double &b);

class String toString(const class Object &v);
class String toString(const char *v);
class String toString(const void *v);
class String toString(const uint8_t &v);
class String toString(const int8_t &v);
class String toString(const uint16_t &v);
class String toString(const int16_t &v);
class String toString(const uint32_t2 &v);
class String toString(const int32_t2 &v);
class String toString(const uint64_t &v);
class String toString(const int64_t &v);
class String toString(const float &v);
class String toString(const double &v);

#ifdef __DJGPP__
uint32_t2 getHash(const uint32_t &v);
uint32_t2 getHash(const int32_t &v);
bool equals(const uint32_t &a,const uint32_t &b);
bool equals(const int32_t &a,const int32_t &b);
class String toString(const uint32_t &v);
class String toString(const int32_t &v);
#endif // __DJGPP__

#endif //__OBJECT_HPP__