#ifndef __PAIR_HPP__                  
#define __PAIR_HPP__

#include "object.hpp"
#include "string.hpp"

template<class KEY, class VALUE>
class KeyValue {
public: 
  KEY key;
  VALUE value;

  KeyValue() {
  }

  KeyValue(const KEY &key, const VALUE &value) {
    this->key = key;
    this->value = value;
  }

  KeyValue(const KeyValue &a) {
    this->key = a.key;
    this->value = a.value;
  }
};


template<class FIRST, class SECOND>
class Pair : public Object {
public:
  FIRST first;
  SECOND second;

  Pair() {
  }

  Pair(const FIRST &first,const SECOND &second) {
    this->first = first;
    this->second = second;
  }

  Pair(const KeyValue<FIRST,SECOND> &a) {
    first = a.key;
    second = a.value;
  }

  Pair(const Pair<FIRST,SECOND> &a) {
    first = a.first;
    second = a.second;
  }

  const Pair<FIRST,SECOND> &operator=(const KeyValue<FIRST,SECOND> &a) {
    first = a.key;
    second = a.value;
    return *this;
  }

  virtual ~Pair() {}

  virtual int32_t hash() const {
    return getHash(first) + getHash(second)*3;
  }
  
  virtual bool equals(Object *a) const {
    Pair<FIRST,SECOND> *v = dynamic_cast<Pair<FIRST,SECOND>*>(a);
    if (v == NULL) return false;
    return ::equals(first,v->first) && ::equals(second,v->second);
  }
  
  virtual String toString() const {
    return "[" + ::toString(first) + ":" + ::toString(second) + "]";
  }
};

#endif //__PAIR_HPP__