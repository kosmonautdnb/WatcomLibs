#ifndef __SPOINTER_HPP__
#define __SPOINTER_HPP__

#include <stdlib.h>

// Please use the constructor with different C pointers only, not one and the same C pointer in different SharedPointers
// Usage: char *a = new char[10]; sprintf(a,"World"); SharedPointer<char> b = SharedPointer<char>(a,true);
// use setDebug() to find refCount problems, the refCount object may have been deleted already when you acces a different pointer wrongly..
// Accessing pointers with this is pretty slow in WatcomC++ 11.0

#define REFCOUNTEDOBJECTISDELETED -0x4000

template<class T>
class RefCountedObject {
public:
  T *ptr;
  int count;
  bool debug;
  bool array;

  RefCountedObject(T *ptr = NULL, bool array = false) : ptr(ptr), array(array), count(1), debug(false) {;}

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

  const SharedPointer<T> &operator=(const SharedPointer<T> &b) {
    RefCountedObject<T> *refBefore = ref;
    ref = b.ref;
    addRef();
    if (refBefore != NULL) refBefore->release();
    return *this;
  }

  T* ptr() {
    return ref == NULL || ref->count <= 0 ? NULL : ref->ptr;
  }

  const T* ptr() const {
    return ref == NULL || ref->count <= 0 ? NULL : ref->ptr;
  }

  operator T*() {
    return ptr();
  }

  operator const T*() const {
    return ptr();
  }

  T* operator->() {
    return ptr();
  }

  const T* operator->() const {
    return ptr();
  }

  bool operator==(const SharedPointer<T> &b) {if (ref == NULL || b.ref == NULL) return ref == NULL && b.ref == NULL; return ref->ptr == b.ref->ptr;}
  bool operator!=(const SharedPointer<T> &b) {return !(*this == b);}

  bool operator==(const T *b) {if (ref == NULL) return false; return ref->ptr == b;}
  bool operator!=(const T *b) {return !(*this == b);}

  SharedPointer<T> &operator()(T *b) {*this = SharedPointer<T>(b); return *this;} // no cArray here

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

template<class T>
class WeakPointer {
public:
  RefCountedObject<T> *ref;

  WeakPointer(T *t = NULL, bool cArray = false) {
    ref = new RefCountedObject<T>(t, cArray); // this will never be deleted (you need to delete the pointer yourself) (maybe you need a weak pointer of "this")
  }

  WeakPointer(const SharedPointer<T> &t) { // normal way of creating a weak pointer
    ref = t.ref;
  }

  WeakPointer(const WeakPointer<T> &b) {
    ref = b.ref;
  }

  ~WeakPointer() {
    ref = NULL;
  }

  void set(SharedPointer<T> &b) {
    ref = b.ref;
  }

  void set(WeakPointer<T> &b) {
    ref = b.ref;
  }

  void clear() {
    ref = NULL;
  }

  const WeakPointer<T> &operator=(const WeakPointer<T> &b) {
    ref = b.ref;
    return *this;
  }

  const WeakPointer<T> &operator=(const SharedPointer<T> &b) {
    ref = b.ref;
    return *this;
  }

  T* ptr() {
    return ref == NULL || ref->count <= 0 ? NULL : ref->ptr;
  }

  const T* ptr() const {
    return ref == NULL || ref->count <= 0 ? NULL : ref->ptr;
  }

  operator T*() {
    return ptr();
  }

  operator const T*() const {
    return ptr();
  }

  T* operator->() {
    return ptr();
  }

  const T* operator->() const {
    return ptr();
  }

  SharedPointer<T> shared() {
    SharedPointer<T> r;
    r.ref = ref;
    r.addRef();
    return r;
  }

  const SharedPointer<T> shared() const {
    SharedPointer<T> r;
    r.ref = ref;
    r.addRef();
    return r;
  }

  static WeakPointer<T> from(SharedPointer<T> &b) {
    WeakPointer<T> r; 
    r.ref = b.ref; 
    return r;
  }

  static const WeakPointer<T> from(const SharedPointer<T> &b) {
    WeakPointer<T> r; 
    r.ref = b.ref; 
    return r;
  }

  bool operator==(const SharedPointer<T> &b) {if (ref == NULL || b.ref == NULL) return ref == NULL && b.ref == NULL; return ref->ptr == b.ref->ptr;}
  bool operator!=(const SharedPointer<T> &b) {return !(*this == b);}

  bool operator==(const WeakPointer<T> &b) {if (ref == NULL || b.ref == NULL) return ref == NULL && b.ref == NULL; return ref->ptr == b.ref->ptr;}
  bool operator!=(const WeakPointer<T> &b) {return !(*this == b);}

  bool operator==(const T *b) {if (ref == NULL) return false; return ref->ptr == b;}
  bool operator!=(const T *b) {return !(*this == b);}

  int refs() {
    return ref == NULL? REFCOUNTEDOBJECTISDELETED : ref->count;
  }
};

template<class TO, class FROM>
class sharedPointer {
public:
  static const SharedPointer<TO> convert(const SharedPointer<FROM> &from) {
    SharedPointer<TO> to; 
    to.ref = reinterpret_cast<RefCountedObject<TO>*>(from.ref); 
    to.addRef(); 
    return to;
  }

  static SharedPointer<TO> convert(SharedPointer<FROM> &from) {
    SharedPointer<TO> to; 
    to.ref = reinterpret_cast<RefCountedObject<TO>*>(from.ref); 
    to.addRef(); 
    return to;
  }

  static SharedPointer<TO> convert(FROM *from) {
    return SharedPointer<TO>(dynamic_cast<TO*>(from));
  }

  static const SharedPointer<TO> convert(const FROM *from) {
    return SharedPointer<TO>(dynamic_cast<TO*>(from));
  }

  static WeakPointer<TO> convert(WeakPointer<FROM> &from) {
    WeakPointer<TO> to; 
    to.ref = reinterpret_cast<RefCountedObject<TO>*>(from.ref); 
    return to;
  }

  static const WeakPointer<TO> convert(const WeakPointer<FROM> &from) {
    WeakPointer<TO> to; 
    to.ref = reinterpret_cast<RefCountedObject<TO>*>(from.ref); 
    return to;
  }

  static const WeakPointer<TO> weak(const SharedPointer<FROM> &from) {
    WeakPointer<TO> to; 
    to.ref = reinterpret_cast<RefCountedObject<TO>*>(from.ref); 
    return to;
  }

  static WeakPointer<TO> weak(SharedPointer<FROM> &from) {
    WeakPointer<TO> to; 
    to.ref = reinterpret_cast<RefCountedObject<TO>*>(from.ref); 
    return to;
  }

  static SharedPointer<TO> shared(WeakPointer<FROM> &from) {
    SharedPointer<TO> to; 
    to.ref = reinterpret_cast<RefCountedObject<TO>*>(from.ref); 
    to.addRef(); 
    return to;
  }

  static const SharedPointer<TO> shared(const WeakPointer<FROM> &from) {
    SharedPointer<TO> to; 
    to.ref = reinterpret_cast<RefCountedObject<TO>*>(from.ref); 
    to.addRef(); 
    return to;
  }

  static SharedPointer<TO> shared(FROM *from) {
    return convert(from);
  }

  static const SharedPointer<TO> shared(const FROM *from) {
    return convert(from);
  }
};

#endif // __SPOINTER__