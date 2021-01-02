#include "symtab.h"
#include "ast-visit.h"
#include "ast-symtabctor.h"
#include "cast.h"
#include "hash.h"
#include "info.h"
#include <search.h>
#include <stdio.h>

static Cons curtab = NULL;
static Hash symtab = NULL;

#define attr_iter(node, attr, iter) \
  for(Cons iter = node->attr; iter; iter = iter->cdr)
#define iter_nodes_attr(node, attr, visitor) \
  for(Cons it = node->attr; it; it = it->cdr) \
    visit((Node)it->car, visitor);

extern int pass_error;

#define semaerr(fmt, ...) \
  fprintf(stderr, fmt, ##__VA_ARGS__), pass_error = 1

Hash SymbolTable(Node node){
  symtab = newHash();
  visit(node, symtabctor);
  return symtab;
}

PASS_DEF(symtabctor);

PASS_HANDLE(symtabctor, Node          , node){}

PASS_HANDLE(symtabctor, ProgNode      , node){
  push_scope(&curtab);
  insert(symtab, node, curtab);
  add_symbol(curtab, node->name, NULL, NULL, node);
  visitChildren(node, symtabctor);
  pop_scope(&curtab);
}

PASS_HANDLE(symtabctor, FunctionNode  , node){

  insert(symtab, node, curtab);
  if(add_symbol(curtab, node->name, node->type, node->param, node))
    semaerr(REDEF_FUN, node->loc.first_line, node->loc.first_column, node->name);

  push_scope(&curtab);
  attr_iter(node, param, it){
    attr_iter(dn(it->car), vars, v){
      insert(symtab, v->car, curtab);
      if(add_symbol(curtab, vn(v->car)->name, dn(it->car)->type, NULL, vn(v->car)))
        semaerr(REDEF_ARG,
            node->loc.first_line,
            node->loc.first_column,
            vn(v->car)->name);
        visitChildren(v->car, symtabctor);
    }
  }
  // to identify param and var
  //visitChildren(node, symtabctor); // inlucde param
  iter_nodes_attr(node, decl_vars, symtabctor);
  iter_nodes_attr(node, decl_funcs, symtabctor);
  visit(node->stmt, symtabctor);
  pop_scope(&curtab);
}

PASS_HANDLE(symtabctor, DeclNode      , node){
  insert(symtab, node, curtab);
  visitChildren(node, symtabctor);
}

PASS_HANDLE(symtabctor, CompStmtNode  , node){
  insert(symtab, node, curtab);
  visitChildren(node, symtabctor);
}

PASS_HANDLE(symtabctor, BinExprNode   , node){
  insert(symtab, node, curtab);
  visitChildren(node, symtabctor);
}

PASS_HANDLE(symtabctor, UnaExprNode   , node){
  insert(symtab, node, curtab);
  visitChildren(node, symtabctor);
}

PASS_HANDLE(symtabctor, AssignNode    , node){
  insert(symtab, node, curtab);
  visitChildren(node, symtabctor);
}

PASS_HANDLE(symtabctor, VarNode       , node){
  insert(symtab, node, curtab);
  if(add_symbol(curtab, node->name, node->type, NULL, node))
      semaerr(REDEF_VAR,
          node->loc.first_line,
          node->loc.first_column,
          node->name);
  visitChildren(node, symtabctor);
}

PASS_HANDLE(symtabctor, VarRefNode    , node){
  insert(symtab, node, curtab);
  visitChildren(node, symtabctor);
}

PASS_HANDLE(symtabctor, ValNode       , node){
  insert(symtab, node, curtab);
  visitChildren(node, symtabctor);
}

PASS_HANDLE(symtabctor, InvocationNode, node){
  insert(symtab, node, curtab);
  visitChildren(node, symtabctor);
}

PASS_HANDLE(symtabctor, IfTestNode    , node){
  insert(symtab, node, curtab);
  visitChildren(node, symtabctor);
}

PASS_HANDLE(symtabctor, WhileNode     , node){
  insert(symtab, node, curtab);
  visitChildren(node, symtabctor);
}
