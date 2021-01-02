#ifndef __HASH_H__
#define __HASH_H__

typedef void** Hash;

typedef struct{
  void *key;
  void *value;
} *Item, ItemStr;

Item newItem(void *k, void *v);

Hash newHash();
void delHash(Hash h);
void *find(Hash hash, void *key);
void earse(Hash hash, void *key);
void insert(Hash hash, void *key, void *value);
void walk(Hash hash, void (*func)(Item));

#endif
