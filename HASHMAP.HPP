#ifndef __HASHMAP_HPP__
#define __HASHMAP_HPP__

#include "array.hpp"
#include "types.hpp"

#define HashMap_ELEMENTCOUNT 509 // prime number

template<class KEY, class VALUE>
class ElementLink {
public:
  KEY key;
  VALUE value;
  ElementLink<KEY,VALUE> *next;
  ElementLink<KEY,VALUE> *last;
  ElementLink() {
    next = NULL;
    last = NULL;
  }
};
                                          
template<class KEY, class VALUE>
class HashMap {
public:
  ElementLink<KEY,VALUE> *elements[HashMap_ELEMENTCOUNT];
  HashMap() {
    for (int i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      elements[i] = NULL;
    }
  }

  virtual ~HashMap() {
    for (int i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      ElementLink<KEY,VALUE> *t = elements[i];
      while(t != NULL) {
        ElementLink<KEY,VALUE> *d = t;
        t = t->next;
        delete d;
      }
    }
  }

  void clear() {
    for (int i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      ElementLink<KEY,VALUE> *t = elements[i];
      while(t != NULL) {
        ElementLink<KEY,VALUE> *d = t;
        t = t->next;
        delete d;
      }
      elements[i]=NULL;
    }
  }

  bool empty() {
    for (int i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      if (elements[i] != NULL) 
        return false;
    }
    return true;
  }

  const VALUE at(const KEY& key) const {
    int hash = getHash(key) % HashMap_ELEMENTCOUNT;
    ElementLink<KEY,VALUE> *t = elements[hash];
    while(t != NULL)  {
      if (t->key == key) {
        return t->value;
      }
      t = t->next;
    }
    return VALUE();
  }

  const VALUE operator[](const KEY& key) const {
    return at(key);
  }

  VALUE &at(const KEY& key) {
    int hash = getHash(key) % HashMap_ELEMENTCOUNT;
    ElementLink<KEY,VALUE> *t = elements[hash];
    ElementLink<KEY,VALUE> *f = NULL;
    while(t != NULL)  {
      if (t->key == key) {
        f = t;
        return t->value;
      }
      t = t->next;
    }
    t = new ElementLink<KEY,VALUE>();
    t->key = key;
    if (f != NULL) {
      ElementLink<KEY,VALUE> *k = f->next; // to keep keys with the very same key one by another
      f->next = t;
      t->next = k;
      t->last = f;
    } else {
      f = elements[hash];
      if (f != NULL) {
        ElementLink<KEY,VALUE> *k = f->next;
        f->next = t;
        t->next = k;
        t->last = f;
      } else {
        elements[hash] = t;
      }
    }
    return t->value;
  }

  VALUE &operator[](const KEY& key) {
    return at(key);
  }

  void insert(const KEY &key, const VALUE &value) {
    (*this)[key] = value;
  }

  bool has(const KEY& key) const {
    int hash = getHash(key) % HashMap_ELEMENTCOUNT;
    ElementLink<KEY,VALUE> *t = elements[hash];
    while(t != NULL)  {
      if (t->key == key) {
        return true;
      }
      t = t->next;
    }
    return false;
  }

  bool erase(const KEY& key) {
    int hash = getHash(key) % HashMap_ELEMENTCOUNT;
    ElementLink<KEY,VALUE> *t = elements[hash];
    while(t != NULL)  {
      if (t->key == key) {
        if (t->last != NULL) t->last->next = t->next;
        if (t->next != NULL) t->next->last = t->last;
        delete t;
        return true;
      }
      t = t->next;
    }
    return false;
  }

  Array<KEY> keys() {
    Array<KEY> r;
    KEY lastKey = KEY();
    bool first = true;
    for (int i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      ElementLink<KEY,VALUE> *t = elements[i];
      while(t != NULL) {
        if ((t->key != lastKey) || first) {
          r.push_back(t->key);
          lastKey = t->key;
          first = false;
        }
        t = t->next;
      }
    }
    return r;
  }

  Array<VALUE> values() {
    Array<VALUE> r;
    for (int i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      ElementLink<KEY,VALUE> *t = elements[i];
      while(t != NULL) {
        r.push_back(t->value);
        t = t->next;
      }
    }
    return r;
  }

  Array<ElementLink<KEY,VALUE> *> entries() {
    Array<ElementLink<KEY,VALUE> *> r;
    for (int i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      ElementLink<KEY,VALUE> *t = elements[i];
      while(t != NULL) {
        r.push_back(t);
        t = t->next;
      }
    }
    return r;
  }

  int64_t size() {
    int64_t r = 0;
    for (int i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      ElementLink<KEY,VALUE> *t = elements[i];
      while(t != NULL) {
        r++;
        t = t->next;
      }
    }
    return r;
  }
};

int getHash(const class String &v);
int getHash(const int v);

#endif //__HASHMAP_HPP__