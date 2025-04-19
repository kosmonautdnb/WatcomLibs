#ifndef __SET_HPP__
#define __SET_HPP__

#include "types.hpp"
#include "object.hpp"

#define Set_ELEMENTCOUNT 509 // prime number

template<class KEY>
class HElementLink {
public:
  KEY key;
  HElementLink<KEY> *next;
  HElementLink<KEY> *last;
  HElementLink() {
    next = NULL;
    last = NULL;
  }
};

template<class KEY>
class Set {
public:
  HElementLink<KEY> *elements[Set_ELEMENTCOUNT];
  Set() {
    for (int32_t i = 0; i < Set_ELEMENTCOUNT; i++) {
      elements[i] = NULL;
    }
  }

  const Set<KEY> &operator=(const Set<KEY> &b) {
    clear();
    for (int32_t i = 0; i < Set_ELEMENTCOUNT; i++) {
      HElementLink<KEY> *t = b.elements[i];
      while(t != NULL) {
        insert(t->key);
        t = t->next;
      }
    }
    return *this;
  }

  virtual ~Set() {
    for (int32_t i = 0; i < Set_ELEMENTCOUNT; i++) {
      HElementLink<KEY> *t = elements[i];
      while(t != NULL) {
        HElementLink<KEY> *d = t;
        t = t->next;
        delete d;
      }
    }
  }

  void clear() {
    for (int32_t i = 0; i < Set_ELEMENTCOUNT; i++) {
      HElementLink<KEY> *t = elements[i];
      while(t != NULL) {
        HElementLink<KEY> *d = t;
        t = t->next;
        delete d;
      }
      elements[i]=NULL;
    }
  }

  bool empty() {
    for (int32_t i = 0; i < Set_ELEMENTCOUNT; i++) {
      if (elements[i] != NULL) 
        return false;
    }
    return true;
  }

  const bool has(const KEY& key) const {
    int32_t hash = getHash(key) % Set_ELEMENTCOUNT;
    HElementLink<KEY> *t = elements[hash];
    while(t != NULL)  {
      if (equals(t->key,key)) {
        return true;
      }
      t = t->next;
    }
    return false;
  }

  const bool operator[](const KEY& key) const {
    return has(key);
  }

  void at(const KEY& key) { // enter if not there
    int32_t hash = getHash(key) % Set_ELEMENTCOUNT;
    HElementLink<KEY> *t = elements[hash];
    HElementLink<KEY> *f = NULL;
    while(t != NULL)  {
      if (equals(t->key,key)) {
        f = t;
        return;
      }
      t = t->next;
    }
    t = new HElementLink<KEY>();
    t->key = key;
    if (f != NULL) {
      HElementLink<KEY> *k = f->next; // to keep keys with the very same key one by another
      f->next = t;
      t->next = k;
      t->last = f;
    } else {
      f = elements[hash];
      if (f != NULL) {
        HElementLink<KEY> *k = f->next;
        f->next = t;
        t->next = k;
        t->last = f;
      } else {
        elements[hash] = t;
      }
    }
    return;
  }

  void operator[](const KEY& key) {
    at(key);
  }

  void insert(const KEY &key) {
    (*this)[key];
  }

  bool erase(const KEY& key) {
    int32_t hash = getHash(key) % Set_ELEMENTCOUNT;
    HElementLink<KEY> *t = elements[hash];
    while(t != NULL)  {
      if (equals(t->key, key)) {
        if (t->last != NULL) t->last->next = t->next;
        if (t->next != NULL) t->next->last = t->last;
        delete t;
        return true;
      }
      t = t->next;
    }
    return false;
  }

  Array<KEY> array() {
    Array<KEY> r;
    KEY lastKey = KEY();
    bool first = true;
    for (int32_t i = 0; i < Set_ELEMENTCOUNT; i++) {
      HElementLink<KEY> *t = elements[i];
      while(t != NULL) {
        if ((!equals(t->key,lastKey)) || first) {
          r.push_back(t->key);
          lastKey = t->key;
          first = false;
        }
        t = t->next;
      }
    }
    return r;
  }

  int64_t size() {
    int64_t r = 0;
    for (int32_t i = 0; i < Set_ELEMENTCOUNT; i++) {
      HElementLink<KEY> *t = elements[i];
      while(t != NULL) {
        r++;
        t = t->next;
      }
    }
    return r;
  }
};

#endif //__SET_HPP__