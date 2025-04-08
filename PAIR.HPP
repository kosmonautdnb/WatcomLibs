#ifndef __PAIR_HPP__                  
#define __PAIR_HPP__

#include "object.hpp"
#include "string.hpp"

template<class FIRST, class SECOND>
class Pair : public Object {
public:
  FIRST first;
  SECOND second;

  Pair() {
  }

  Pair(const FIRST &_a,const SECOND &_b) {
    first = _a;
    second = _b;
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