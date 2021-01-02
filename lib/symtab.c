#include "ast.h"
#include "info.h"
#include "symtab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>

#undef add_symbol

#define FUNC_OVERLOADING 1

/*
   1. every time a new symbol is entered into the symbol table
   2. every time a new scope is generated
   3. every time a scope is closed
   4. every time when a symbol table is created
   5. every time when a symbol table is destroyed
   6. dump a symbol table when the symbol table is destroyed
   7. dump all symbol tables at the end of the program
   */

DEFCONS(SymEntry,
    char *, sym,
    Type , type,
    Cons , args,
    Node , node,
    int, index,
    int, findex
);

void push_scope(Cons *tab){
  *tab = newCons(NULL, *tab);
  SHOW_NEWSCP();
}

void pop_scope(Cons *tab){
  SHOW_CLSSCP();
  dump_symtab(*tab);
  *tab = (*tab)->cdr;
}

#define one_of_null(a, b) \
  ((!(a) && (b)) || (!(b) && a))

int add_symbol(Cons tabs, void *sym, Type t, Cons args, Node node){
  static int issue_function = 0;
  Cons *i = (Cons*)&(tabs->car);
  int issue_index = *i ? ((SymEntry)(*(Cons*)&(tabs->car))->car)->index + 1 : 0;
  while(*i){
    if(EQS(((SymEntry)(*i)->car)->sym, (char*)sym)){
#if FUNC_OVERLOADING
      // only one var, one func, different type allow
      int allow = (node->nt != ((SymEntry)(*i)->car)->node->nt)
                && !same_type(t, ((SymEntry)(*i)->car)->type)
                && one_of_null(args, ((SymEntry)(*i)->car)->args);
      if(!allow) return 1;
#else
      return 1;
#endif
    }
    i = &((*i)->cdr);
  }

  if(t && t->t == REAL_TYPE){
    if(issue_index % REAL_SIZE)
      issue_index += issue_index % REAL_SIZE;
  }

  tabs->car = newCons(newSymEntry((char*)sym,
        t, args, node, issue_index,
        node->nt == Function ? issue_function++ : 0),
      tabs->car);

  SHOW_NEWSYM((char*)sym);
  return 0;
}

SymEntry lookup_symbol_lv(Cons tabs, void *sym, Type t, int *lv, int inv){
  for(Cons tab = tabs; tab; tab = tab->cdr)
    for(Cons e = (Cons)(tab->car); e; e = e->cdr)
      if(EQS(((SymEntry)(e->car))->sym, (char*)sym)){
        if(inv > 0 && ((SymEntry)(e->car))->node->nt != Function)
          continue;
        if(inv < 0 && ((SymEntry)(e->car))->node->nt == Function)
          continue;
        if(!t){
          if(lv) *lv = symtab_lv(tab);
          return (SymEntry)(e->car);
        }
        else if(t->t == ((SymEntry)(e->car))->type->t){
          if(lv) *lv = symtab_lv(tab);
          return (SymEntry)(e->car);
        }
      }
  if(lv) *lv = -1;
  return NULL;
}

void dump_symtab(Cons tab){
  int lv = -1;
  char buffer[128];
  for(Cons c = tab; c; c = c->cdr) lv++;
  SHOW_SYMTAB_HEAD();
  for(Cons t = tab; t; t = t->cdr, lv--){
    for(Cons c = (Cons)t->car; c; c = c->cdr){
      SymEntry se = (SymEntry)c->car;
      printf(SYMTAB_ENTRY_FMT,
          se->sym, lv, funcOvar2str(se->type, se->args, buffer));
    }
  }
  SHOW_SYMTAB_TAIL();
}

int symtab_lv(Cons tab){
  int lv = -1;
  while(tab) lv++, tab = tab->cdr;
  return lv;
}
