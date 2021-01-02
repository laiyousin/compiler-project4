#ifndef __SYMTAB_H__
#define __SYMTAB_H__

#define IGNORE_CASE 1
#if IGNORE_CASE
#include <strings.h>
#define EQS(a, b) (!strcasecmp(a, b))
#else
#define EQS(a, b) (!strcmp(a, b))
#endif

#include "ast.h"

typedef struct {
  char *sym;
  Type type;
  Cons args;
  Node node;
  int index;
  int findex;
} *SymEntry, SymEntryStr;

DECCONS(SymEntry,
  char *, sym,
  Type , type,
  Cons , args,
  Node , node,
  int, index,
  int, findex
);

void push_scope(Cons *tab);
void pop_scope(Cons *tab);

int add_symbol(Cons tab, void *sym, Type t, Cons args, Node node);
#define add_symbol(a, b, c, d, e) add_symbol(a, b, c, d, (Node)e)
#define find_symbol(tabs, sym, t) lookup_symbol_lv(tabs, sym, t, NULL, 0)
#define lookup_symbol(tabs, sym, t, inv) lookup_symbol_lv(tabs, sym, t, NULL, inv)
#define find_symbol_lv(tabs, sym, t, lv) lookup_symbol_lv(tabs, sym, t, lv, 0)

SymEntry lookup_symbol_lv(Cons tabs, void *sym, Type t, int *lv, int inv);

void dump_symtab(Cons symtab);
int symtab_lv(Cons tab);

#endif
