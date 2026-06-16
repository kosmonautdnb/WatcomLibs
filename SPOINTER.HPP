#ifndef __SPOINTER_HPP__
#define __SPOINTER_HPP__

#include <stdlib.h>

// Please use the constructor with different C pointers only, not one and the same C pointer in different SharedPointers
// !!!Just a prototype not really tested, yet!!!
// WatcomC++ 11.0

// Usage: char *a = new char[10]; sprintf(a,"World"); SharedPointer<char> b = SharedPointer<char>(a);

class RefCount {
public:
  RefCount(bool debug = false) : count(1), debug(debug) {;}
  bool debug;
  int count;
};

template<class T>
class SharedPointer {
public:
  T *ptr;
  RefCount *refs;

  SharedPointer(T *t = NULL, bool debug = false) {
    ptr = t; 
    refs = new RefCount(debug);
  }

  ~SharedPointer() {
    release();
  }
  
  SharedPointer(const SharedPointer<T> &b) {
    ptr = b.ptr; 
    refs = b.refs;
    addRef();
  }

  const SharedPointer<T> &operator=(SharedPointer<T> &b) {
    T *ptrBefore = ptr;
    RefCount *refsBefore = refs;
    ptr = b.ptr; 
    refs = b.refs;
    addRef();
    refsBefore->count--;
    if (refsBefore->count == 0) {
      if (ptrBefore != NULL) {delete ptrBefore; ptrBefore = NULL;}
      if (refsBefore != NULL && (!refsBefore->debug)) {delete refsBefore; refsBefore = NULL;}
    }
    return *this;
  }

  operator T*() {
    if (refs->count<=0) return NULL;
    return ptr;
  }

  void addRef() {
    refs->count++;
  }

  bool release() {
    if (decRef() == 0) {
      if (ptr != NULL) {delete ptr; ptr = NULL;}
      if (refs != NULL && (!refs->debug)) {delete refs; refs = NULL;}
      return true;
    } 
    return false;
  }

  int decRef() {
    refs->count--;
    return refs->count;
  }

  int references() {
    return refs->count;
  }
};

#endif // __SPOINTER__