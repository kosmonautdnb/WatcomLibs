#ifndef __SPOINTER_HPP__
#define __SPOINTER_HPP__

#include <stdlib.h>

// Please use the constructor with different C pointers only, not one and the same C pointer in different SharedPointers
// Usage: char *a = new char[10]; sprintf(a,"World"); SharedPointer<char> b = SharedPointer<char>(a,true);
// use setDebug() to find refCount problems, the refCount object may have been deleted already when you acces a different pointer wrongly..

#define REFCOUNTEDOBJECTISDELETED -0x4000

template<class T>
class RefCountedObject {
public:
  T *ptr;
  int count;
  bool debug;
  bool array;

  RefCountedObject(T *ptr, bool array) : ptr(ptr), array(array), count(1), debug(false) {;}

  void del() {
    if (ptr != NULL) {
      if (!array) delete ptr;
      if (array) delete[] ptr;
      ptr = NULL;
    }
    count = REFCOUNTEDOBJECTISDELETED;
    if (!debug) {
      delete this;
    }
  }

  bool addRef() {
    if (count != REFCOUNTEDOBJECTISDELETED) count++;
    return true;
  }

  bool release() {
    if (decRef() == 0) {
      del();
      return true;
    } 
    return false;
  }

  int decRef() {
    if (count != REFCOUNTEDOBJECTISDELETED) count--;
    return count;
  }
};

template<class T>
class SharedPointer {
public:
  RefCountedObject<T> *ref;

  SharedPointer(T *t = NULL, bool cArray = false) {
    ref = new RefCountedObject<T>(t, cArray);
  }

  ~SharedPointer() {
    release();
  }
  
  SharedPointer(const SharedPointer<T> &b) {
    ref = b.ref;
    addRef();
  }

  const SharedPointer<T> &operator=(SharedPointer<T> &b) {
    RefCountedObject<T> *refBefore = ref;
    ref = b.ref;
    addRef();
    if (refBefore != NULL) refBefore->release();
    return *this;
  }

  operator T*() {
    return ref == NULL || ref->count <= 0 ? NULL : ref->ptr;
  }

  bool addRef() {
    return ref == NULL ? false : ref->addRef();
  }

  bool release() {
    return ref == NULL ? false : ref->release();
  }

  int decRef() {
    return ref == NULL ? REFCOUNTEDOBJECTISDELETED : ref->decRef();
  }

  int refs() {
    return ref == NULL? REFCOUNTEDOBJECTISDELETED : ref->count;
  }

  bool setDebug(bool debug = true) {
    if (ref == NULL) return false;
    ref->debug = debug;
    if (!debug) {
      if (ref->count == REFCOUNTEDOBJECTISDELETED) {
        ref->del(); 
        ref = NULL;
        return true;
      }
    }
    return false;
  }
};

#endif // __SPOINTER__