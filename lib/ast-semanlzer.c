#include "symtab.h"
#include "ast-visit.h"
#include "ast-semanlzer.h"
#include "cast.h"
#include "hash.h"
#include "info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Hash symtab;

/*
   - array access error (5%) // too much or too less(?)
   - redefined error (9%)    // divide func or var
   - return value error (9%) // check if exist ret value assignment
   - type error (9%)         // type consistant
   - undeclared error (9%)   // divide func or var
   - uninitialized error (9%) // ignore (consider)
   */

/*
   v1.  Declare two variables with identical names in two scopes.
   Can you find the appropriate variable when that name is used in an expression?
   v2.  Declare two variables with identical names in the same scope.
   Can you catch the duplicate declarations?
   v3.  Declare a variable and a function with identical names in the same scope.
   Can you catch the duplicate declarations?
   4.  Declare a variable and a function with identical names in the differnt scopes.
   Can you find the appropriate usage when that name is used?
   v 5.  Undeclared variables
   o 6.  Undeclared types (not in this mini-pascal project)
   o 7.  Undeclared functions (not in this mini-pascal project)
   o 8.  Undeclared classes (not in this mini-pascal project)
   v 9.  Arguments’ types and numbers are wrong.
   v 10. Type errors in arithmetic expressions
   v 11. Redeclared variables
   o 12. Redeclared types (not in this mini-pascal project)
   v 13. Redeclared functions
   o 14. Redeclared classes (not in this mini-pascal project)
   o 15. Cycles in inheritance hierarchy (not in this mini-pascal project)
   o 16. Cycles in structure containment (not in this mini-pascal project)
   */

#define attr_iter(node, attr, iter) \
  for(Cons iter = node->attr; iter; iter = iter->cdr)

extern int pass_error;
int smerr = 0;

#define semaerr(fmt, ...) \
  fprintf(stderr, fmt, ##__VA_ARGS__), pass_error = 1, smerr = 1

Cons func_ret = NULL;
Cons func_cur = NULL;

void SemanticCheck(Node node, Hash tab){
  symtab = tab;
  visit(node, semanlzer);
}

PASS_DEF(semanlzer);

PASS_HANDLE(semanlzer, Node          , node){}

PASS_HANDLE(semanlzer, ProgNode      , node){
  visitChildren(node, semanlzer);
}

PASS_HANDLE(semanlzer, FunctionNode  , node){
  // we only do reutrn check when it is a function (not procedure)
  if(node->type){
    func_ret = newCons(node, func_ret);
    func_cur = newCons(node, func_cur);
  }
  visitChildren(node, semanlzer);
  if(node->type && func_ret && func_ret->car == node){
    semaerr(RETURN_VAL, node->loc.first_line, node->loc.first_column, (char*)node->name);
    Cons rel = func_ret;
    func_ret = func_ret->cdr;
    free(rel);
  }
  if(node->type){
    Cons rel = func_cur;
    func_cur = func_cur->cdr;
    free(rel);
  }
  smerr = 0; // retval check ignore previous check result
}

PASS_HANDLE(semanlzer, DeclNode      , node){
}

PASS_HANDLE(semanlzer, CompStmtNode  , node){
  visitChildren(node, semanlzer);
  smerr = 0;
}

int isBoolean(BinOp op){
  switch(op){
      case Lt:
      case Gt:
      case Eq:
      case Let:
      case Get:
      case Neq:
        return 1;
      default:
        return 0;
    }
}

TypeStr Boolean = {
  .t = INTEGER_TYPE,
  .beg = 0,
  .end = 0,
  .next = NULL
};

PASS_HANDLE(semanlzer, BinExprNode   , node){
  visitChildren(node, semanlzer);
  // if exist previous error, ignore arith type check
  if(smerr) return;
  if(!same_type(node->rval->type, node->lval->type))
    semaerr(ARITH_TYPE, node->loc.first_line, node->loc.first_column, binOpLit[node->op]);
  else
    set_type(node, isBoolean(node->op) ? &Boolean : node->rval->type);
}

PASS_HANDLE(semanlzer, UnaExprNode   , node){
  visitChildren(node, semanlzer);
  set_type(node, node->op == Not ? &Boolean : node->val->type);
}

PASS_HANDLE(semanlzer, AssignNode    , node){

  visitChildren(node, semanlzer);
  SymEntry se = find_symbol(
      find(symtab, node),
      vrn(node->lval)->name,
      vrn(node->lval)->type);
  if(se && func_ret){
    if(se->node == func_ret->car){
      Cons rel = func_ret;
      func_ret = func_ret->cdr;
      free(rel);
    }
  }

  if(!smerr){
    if(se && se->args){
      if(!func_cur || se->node != func_cur->car)
        semaerr(ASSIG_TYPE, node->loc.first_line, node->loc.first_column);
    }
    else if(!same_type(node->rval->type, node->lval->type))
      semaerr(ASSIG_TYPE, node->loc.first_line, node->loc.first_column);
  }
  smerr = 0;
}

PASS_HANDLE(semanlzer, VarNode       , node){
}

Type infer_array_type(Type t, Cons index){
  while(index && t)
    t = t->next, index = index->cdr;
  return index ? NULL : t;
}

PASS_HANDLE(semanlzer, VarRefNode    , node){

  SymEntry se = lookup_symbol(
      find(symtab, node),
      node->name, NULL, node->index ? -1 : 0);

  if(!se){
    if(isReserved(node->name))
      set_type(node, reserveType(node->name));
    else
      semaerr(UNDEC_VAR,
        node->loc.first_line, node->loc.first_column, node->name);
  }
  else if(se->type){
    Type t = infer_array_type(se->type, node->index);
    if(!t) semaerr(INDEX_MANY, node->loc.first_line, node->loc.first_column, node->name);
    else set_type(node, t);
  }

  // TODO: bound check? type check?
  visitChildren(node, semanlzer);
  if(!smerr)
    attr_iter(node, index, iter)
      if(!nd(iter->car)->type ||
          nd(iter->car)->type->t != INTEGER_TYPE){
        semaerr(INDEX_TYPE,
            nd(iter->car)->loc.first_line,
            nd(iter->car)->loc.first_column);
      }
}

PASS_HANDLE(semanlzer, ValNode       , node){
}

int match_param_args(Cons param, Cons args){
  Cons vars = NULL;
  for(; param && args; param = param->cdr)
    for(vars = dn(param->car)->vars; vars && args; vars = vars->cdr, args = args->cdr)
      if(!same_type(vn(vars->car)->type, nd(args->car)->type)) return 0;
  return !param && !args && !vars;
}

PASS_HANDLE(semanlzer, InvocationNode, node){
  SymEntry se = lookup_symbol(
      find(symtab, node),
      node->name, NULL, 1);
  if(!se){
    if(isReserved(node->name))
      set_type(node, reserveType(node->name));
    else
      semaerr(UNDEC_FUN, node->loc.first_line, node->loc.first_column, node->name);
  }
  else set_type(node, se->type);
  visitChildren(node, semanlzer);
  // if args check failed, ignore this check
  if(!smerr && se && !match_param_args(fn(se->node)->param, node->args)){
    semaerr(WRONG_ARGS, node->loc.first_line, node->loc.first_column, node->name);
  }
}

PASS_HANDLE(semanlzer, IfTestNode    , node){
  visitChildren(node, semanlzer);
  smerr = 0;
}

PASS_HANDLE(semanlzer, WhileNode     , node){
  visitChildren(node, semanlzer);
  smerr = 0;
}
