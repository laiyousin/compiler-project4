#ifndef _GNU_SOURCE
#define _GNU_SOURCE     /* Expose declaration of tdestroy() */
#endif

#include <search.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "hash.h"

Hash newHash(){
  return (Hash)calloc(0, sizeof(void*));
}

void delHash(Hash h){
  tdestroy(*h, free);
}

Item newItem(void *k, void *v){
  Item item = (Item)malloc(sizeof(ItemStr));
  item->key = k, item->value = v;
  return item;
}

int
compare(const void *pa, const void *pb)
{
    if (((Item) pa)->key < ((Item) pb)->key)
        return -1;
    if (((Item) pa)->key > ((Item) pb)->key)
        return 1;
    return 0;
}

void*
find(Hash hash, void *key){
  ItemStr same = {.key = key, .value = NULL};
  Item *result = (Item*)tfind(&same, hash, compare);
  return result ? (*result)->value : result;
}

void
earse(Hash hash, void *key){
  ItemStr same = {.key = key, .value = NULL};
  tdelete(&same, hash, compare);
}

void insert(Hash hash, void *key, void *value){
  tsearch(newItem(key, value), hash, compare);
}

void (*walkfunc)(Item);

void
action(const void *nodep, const VISIT which, const int depth)
{
    switch (which) {
    case preorder:
        break;
    case postorder:
        walkfunc(*(Item*) nodep);
        break;
    case endorder:
        break;
    case leaf:
        walkfunc(*(Item*) nodep);
        break;
    }
}

void walk(Hash hash, void (*func)(Item)){
  walkfunc = func; twalk(*hash, action);
}

#if 0
void f(Item datap){
  printf("%p %d\n", datap->key, *(int*)(datap->value));
}

int
main(void)
{
    int i;
    Hash h = newHash();
    srand(time(NULL));
    int data[12];
    Item p;
    for (i = 0; i < 12; i++) {
      data[i] = i;
      insert(h, data + i, data + i);
    }
    walk(h, f);
    p = find(h, data + 5);
    printf("%p %d\n", p->key, *(int*)(p->value));
    exit(EXIT_SUCCESS);
}
#endif
