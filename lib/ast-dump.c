#include "ast-dump.h"
#include "cast.h"
#include <stdio.h>

#define print_node(x, n, fmt, ...) \
  printf("%*c<" #x fmt ">\n", n, ' ', ##__VA_ARGS__)

#define attr_iter(attr, iter) \
  for(Cons iter = node->attr; iter; iter = iter->cdr)

#define iter_nodes_attr(attr) \
  for(Cons it = node->attr; it; it = it->cdr) \
  indent += gap, dump((Node)it->car), indent -= gap;

#define gap 2
int indent = 0;

PASS_DEF(dump);

PASS_HANDLE(dump, Node          , node){}

PASS_HANDLE(dump, ProgNode      , node){
  print_node(prog, indent, ":%s", node->name);
  iter_nodes_attr(decl_vars);
  iter_nodes_attr(decl_funcs);
  indent += gap, dump(node->stmt), indent -= gap;
}

PASS_HANDLE(dump, FunctionNode  , node){
  print_node(func, indent, ":%s", node->name);
  iter_nodes_attr(param);
  iter_nodes_attr(decl_vars);
  iter_nodes_attr(decl_funcs);
  indent += gap, dump(node->stmt), indent -= gap;
}

PASS_HANDLE(dump, DeclNode      , node){
  print_node(decl, indent, ":%s", type2str(NULL, node->type));
  iter_nodes_attr(vars);
}

PASS_HANDLE(dump, CompStmtNode  , node){
  print_node(comp, indent, "");
  iter_nodes_attr(stmts);
}

PASS_HANDLE(dump, BinExprNode   , node){
  print_node(binary, indent, ":%s",
      binOpLit[bn(node)->op]);
  indent += gap, dump(node->lval), indent -= gap;
  indent += gap, dump(node->rval), indent -= gap;
}

PASS_HANDLE(dump, UnaExprNode   , node){
  print_node(unary, indent, ":%s",
      unaOpLit[un(node)->op]);
  indent += gap, dump(node->val), indent -= gap;
}

PASS_HANDLE(dump, AssignNode    , node){
  print_node(assignment, indent, "");
  indent += gap, dump(node->lval), indent -= gap;
  indent += gap, dump(node->rval), indent -= gap;
}

PASS_HANDLE(dump, VarNode       , node){
  print_node(var, indent, ":%s", node->name);
}

PASS_HANDLE(dump, VarRefNode    , node){
  print_node(ref, indent, ":%s", node->name);
  iter_nodes_attr(index);
}

PASS_HANDLE(dump, ValNode       , node){
  print_node(val, indent, ":%s", val2str(node));
}

PASS_HANDLE(dump, InvocationNode, node){
  print_node(invocation, indent, ":%s", node->name);
  iter_nodes_attr(args);
}

PASS_HANDLE(dump, IfTestNode    , node){
  print_node(if, indent, "");
  indent += gap, dump(node->expr), indent -= gap;
  indent += gap, dump(node->then), indent -= gap;
  indent += gap, dump(node->els) , indent -= gap;
}

PASS_HANDLE(dump, WhileNode     , node){
  print_node(while, indent, "");
  indent += gap, dump(node->expr), indent -= gap;
  indent += gap, dump(node->stmt), indent -= gap;
}
