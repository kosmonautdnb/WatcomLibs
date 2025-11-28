#ifndef __ARRAY_HPP__
#define __ARRAY_HPP__

#include <stdlib.h>
#include "types.hpp"

template<class T>
class Array {
public:
  T *data;
  size_t usedSize;
  size_t allocatedSize;

  Array() {
    usedSize = 0;
    allocatedSize = 5;
    data = new T[(unsigned int)allocatedSize]; // no 64 bit new
  }
  
  Array(int size) {
    usedSize = size;
    allocatedSize = (size+5)*1.5;
    data = new T[(unsigned int)allocatedSize]; // no 64 bit new
  }

  const Array &operator=(const Array &b) {
    T *copy = new T[(unsigned int)b.allocatedSize]; // no 64 bit new
    for (size_t i = 0; i < b.usedSize; i++)
      copy[i] = b.data[i];
    if (data != NULL) delete[] data;
    data = copy;
    usedSize = b.usedSize;
    allocatedSize = b.allocatedSize;
    return *this;
  }
  
  ~Array() {
    if (data != NULL) delete[] data;
    usedSize = 0;
    allocatedSize = 0;
  }
  
  void reAlloc(size_t numElements) {
    if (numElements<2) numElements = 2; // *1.5 is a problem
    T *data2 = new T[(unsigned int)numElements]; // no 64 bit new
    for (size_t i = 0; i < numElements; i++)
      data2[i] = i < usedSize ? data[i] : T();
    delete[] data;
    data = data2;
    allocatedSize = numElements;
  }

  void reserve(size_t numElements) {
    if (numElements>allocatedSize)
      reAlloc(numElements);
  }

  void erase(size_t pos, size_t count) {
    size_t i;
    for (i = pos+count; i < usedSize; i++) {
      data[i-count]=data[i];    
    }
    for (i = usedSize-count; i < usedSize; i++) {
      data[i]=T();    
    }
    usedSize-=count;
  }

  void insert(const T &element, size_t pos) {
    resize(usedSize+1);
    size_t i;
    for (i = usedSize-1; i > pos; i--) {
      data[i]=data[i-1];    
    }
    data[pos]=element;    
  }
  
  void resize(size_t numElements) {
    if (numElements<allocatedSize) {
      usedSize = numElements;
    } else {
      reAlloc((size_t)(numElements*1.5));
      usedSize = numElements;
    }
  }
  
  void push_back(const T& elem) {
    if (usedSize < allocatedSize) {
      data[usedSize] = elem;
      usedSize++;
    } else {
      reAlloc((size_t)(allocatedSize*1.5));
      data[usedSize] = elem;
      usedSize++;
    }
  }
  
  void clear() {
    usedSize = 0;
  }
  
  size_t size() const {
    return usedSize;
  }
  
  bool empty() const {
    return size()==0;
  }
  
  T &operator[](size_t index) {
    return data[index];
  }

  const T &operator[](size_t index) const {
    return data[index];
  }
  
  T& back() {
    return data[usedSize-1];
  }
  
  void pop_back() {
    if (usedSize > 0)
      usedSize--;
  }
};

#endif //__ARRAY_HPP__