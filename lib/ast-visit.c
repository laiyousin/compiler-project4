#include "ast-visit.h"
#include "cast.h"
#include <stdio.h>

#undef visitChildren

#define iter_attr(node, attr) \
  for(Cons it = node->attr; it; it = it->cdr)
#define iter_nodes_attr(node, attr, visitor) \
  for(Cons it = node->attr; it; it = it->cdr) \
    visit((Node)it->car, visitor);

PASS_DEF(visit, Visitor);
PASS_DEF(visitChildren, Visitor);

#define DEF_HANDLE(T) \
  MPASS_HANDLE(visit, T ## Node, node, Visitor, visitor){ \
    return visitor(nd(node)); \
  }

MAP(DEF_HANDLE, , NodeTypes);

PASS_HANDLE(visitChildren, Node          , node, Visitor, visitor){}

PASS_HANDLE(visitChildren, ProgNode      , node, Visitor, visitor){
  iter_nodes_attr(node, decl_vars, visitor);
  iter_nodes_attr(node, decl_funcs, visitor);
  visit(node->stmt, visitor);
}

PASS_HANDLE(visitChildren, FunctionNode  , node, Visitor, visitor){
  iter_nodes_attr(node, param, visitor);
  iter_nodes_attr(node, decl_vars, visitor);
  iter_nodes_attr(node, decl_funcs, visitor);
  visit(fn(node)->stmt, visitor);
}

PASS_HANDLE(visitChildren, DeclNode      , node, Visitor, visitor){
  iter_nodes_attr(node, vars, visitor);
}

PASS_HANDLE(visitChildren, CompStmtNode  , node, Visitor, visitor){
  iter_nodes_attr(node, stmts, visitor);
}

PASS_HANDLE(visitChildren, BinExprNode   , node, Visitor, visitor){
  visit(node->lval, visitor);
  visit(node->rval, visitor);
}

PASS_HANDLE(visitChildren, UnaExprNode   , node, Visitor, visitor){
  visit(node->val, visitor);
}

PASS_HANDLE(visitChildren, AssignNode    , node, Visitor, visitor){
  visit(node->lval, visitor);
  visit(node->rval, visitor);
}

PASS_HANDLE(visitChildren, VarNode       , node, Visitor, visitor){
}

PASS_HANDLE(visitChildren, VarRefNode    , node, Visitor, visitor){
  iter_nodes_attr(node, index, visitor);
}

PASS_HANDLE(visitChildren, ValNode       , node, Visitor, visitor){
}

PASS_HANDLE(visitChildren, InvocationNode, node, Visitor, visitor){
  iter_nodes_attr(node, args, visitor);
}

PASS_HANDLE(visitChildren, IfTestNode    , node, Visitor, visitor){
  visit(node->expr, visitor);
  visit(node->then, visitor);
  visit(node->els, visitor) ;
}

PASS_HANDLE(visitChildren, WhileNode     , node, Visitor, visitor){
  visit(node->expr, visitor);
  visit(node->stmt, visitor);
}
