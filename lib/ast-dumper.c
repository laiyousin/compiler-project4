#include "ast-visit.h"
#include "ast-dumper.h"
#include "cast.h"
#include <stdio.h>

#define print_node(x, n, fmt, ...) \
  printf("%*c<" #x fmt ">\n", n, ' ', ##__VA_ARGS__)

#define attr_iter(attr, iter) \
  for(Cons iter = node->attr; iter; iter = iter->cdr)

#define gap 2
int space = 0;

#define wrap_indent(st) \
  space += gap, st, space -= gap

PASS_DEF(dumper);

PASS_HANDLE(dumper, Node          , node){}

PASS_HANDLE(dumper, ProgNode      , node){
  print_node(prog, space, ":%s", node->name);
  wrap_indent(visitChildren(node, dumper));
}

PASS_HANDLE(dumper, FunctionNode  , node){
  print_node(func, space, ":%s", node->name);
  wrap_indent(visitChildren(node, dumper));
}

PASS_HANDLE(dumper, DeclNode      , node){
  print_node(decl, space, ":%s", type2str(NULL, node->type));
  wrap_indent(visitChildren(node, dumper));
}

PASS_HANDLE(dumper, CompStmtNode  , node){
  print_node(comp, space, "");
  wrap_indent(visitChildren(node, dumper));
}

PASS_HANDLE(dumper, BinExprNode   , node){
  print_node(binary, space, ":%s", binOpLit[bn(node)->op]);
  wrap_indent(visitChildren(node, dumper));
}

PASS_HANDLE(dumper, UnaExprNode   , node){
  print_node(unary, space, ":%s", unaOpLit[un(node)->op]);
  wrap_indent(visitChildren(node, dumper));
}

PASS_HANDLE(dumper, AssignNode    , node){
  print_node(assignment, space, "");
  wrap_indent(visitChildren(node, dumper));
}

PASS_HANDLE(dumper, VarNode       , node){
  print_node(var, space, ":%s", node->name);
}

PASS_HANDLE(dumper, VarRefNode    , node){
  print_node(ref, space, ":%s", node->name);
  wrap_indent(visitChildren(node, dumper));
}

PASS_HANDLE(dumper, ValNode       , node){
  print_node(val, space, ":%s", val2str(vln(node)));
}

PASS_HANDLE(dumper, InvocationNode, node){
  print_node(invocation, space, ":%s", node->name);
  wrap_indent(visitChildren(node, dumper));
}

PASS_HANDLE(dumper, IfTestNode    , node){
  print_node(if, space, "");
  wrap_indent(visitChildren(node, dumper));
}

PASS_HANDLE(dumper, WhileNode     , node){
  print_node(while, space, "");
  wrap_indent(visitChildren(node, dumper));
}
