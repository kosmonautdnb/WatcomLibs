#ifndef __HASHMAP_HPP__
#define __HASHMAP_HPP__

#include "types.hpp"
#include "object.hpp"
#include "array.hpp"
#include "pair.hpp"

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
    for (int32_t2 i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      elements[i] = NULL;
    }
  }

  const HashMap<KEY,VALUE> &operator=(const HashMap<KEY,VALUE> &b) {
    clear();
    for (int32_t2 i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      ElementLink<KEY,VALUE> *t = b.elements[i];
      while(t != NULL) {
        insert((const KEY)t->key, t->value);
        t = t->next;
      }
    }
    return *this;
  }

  virtual ~HashMap() {
    for (int32_t2 i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      ElementLink<KEY,VALUE> *t = elements[i];
      while(t != NULL) {
        ElementLink<KEY,VALUE> *d = t;
        t = t->next;
        delete d;
      }
    }
  }

  void clear() {
    for (int32_t2 i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      ElementLink<KEY,VALUE> *t = elements[i];
      while(t != NULL) {
        ElementLink<KEY,VALUE> *d = t;
        t = t->next;
        delete d;
      }
      elements[i]=NULL;
    }
  }

  bool empty() const {
    for (int32_t2 i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      if (elements[i] != NULL) 
        return false;
    }
    return true;
  }

  const VALUE at(const KEY& key) const {
    uint32_t2 hash = getHash(key) % HashMap_ELEMENTCOUNT;
    ElementLink<KEY,VALUE> *t = elements[hash];
    while(t != NULL)  {
      if (equals((const KEY)t->key,key)) {
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
    uint32_t2 hash = getHash(key) % HashMap_ELEMENTCOUNT;
    ElementLink<KEY,VALUE> *t = elements[hash];
    ElementLink<KEY,VALUE> *f = NULL;
    while(t != NULL)  {
      if (equals((const KEY)t->key,key)) {
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

  void insert(const KeyValue<KEY, VALUE> &entry) {
    (*this)[entry.key] = entry.value;
  }

  bool has(const KEY& key) const {
    uint32_t2 hash = getHash(key) % HashMap_ELEMENTCOUNT;
    ElementLink<KEY,VALUE> *t = elements[hash];
    while(t != NULL)  {
      if (equals((const KEY)t->key,key)) {
        return true;
      }
      t = t->next;
    }
    return false;
  }

  bool erase(const KEY& key) {
    uint32_t2 hash = getHash(key) % HashMap_ELEMENTCOUNT;
    ElementLink<KEY,VALUE> *t = elements[hash];
    while(t != NULL)  {
      if (equals((const KEY)t->key, key)) {
        if (t->last != NULL) t->last->next = t->next;
        if (t->next != NULL) t->next->last = t->last;
        delete t;
        return true;
      }
      t = t->next;
    }
    return false;
  }

  Array<KEY> keys() const {
    Array<KEY> r;
    KEY lastKey = KEY();
    bool first = true;
    for (int32_t2 i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      ElementLink<KEY,VALUE> *t = elements[i];
      while(t != NULL) {
        if ((!equals((const KEY)t->key,lastKey)) || first) {
          r.push_back(t->key);
          lastKey = t->key;
          first = false;
        }
        t = t->next;
      }
    }
    return r;
  }

  Array<VALUE> values() const {
    Array<VALUE> r;
    for (int32_t2 i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      ElementLink<KEY,VALUE> *t = elements[i];
      while(t != NULL) {
        r.push_back(t->value);
        t = t->next;
      }
    }
    return r;
  }

  Array<KeyValue<KEY,VALUE> > entries() const {
    Array<KeyValue<KEY,VALUE> > r;
    for (int32_t2 i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      ElementLink<KEY,VALUE> *t = elements[i];
      while(t != NULL) {
        r.push_back(KeyValue<KEY,VALUE>(t->key,t->value));
        t = t->next;
      }
    }
    return r;
  }

  size_t size() const {
    size_t r = 0;
    for (int32_t2 i = 0; i < HashMap_ELEMENTCOUNT; i++) {
      ElementLink<KEY,VALUE> *t = elements[i];
      while(t != NULL) {
        r++;
        t = t->next;
      }
    }
    return r;
  }
};

#endif //__HASHMAP_HPP__