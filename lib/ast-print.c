#include "ast.h"
#include "cast.h"
#include <stdio.h>

#define print_node(x, n, fmt, ...) \
  printf("%*c<" #x fmt ">\n", n, ' ', ##__VA_ARGS__)

#define attr_iter(t, attr, iter) \
  for(Cons iter = t(node)->attr; iter; iter = iter->cdr)

#define iter_nodes_attr(t, attr) \
  for(Cons it = t(node)->attr; it; it = it->cdr) \
    print(it->car, ngap)

#define gap 2
void print(void *node, int id){
  if(!node) return;
  int ngap = id + gap;
  switch(nd(node)->nt){
    case Prog:
      print_node(prog, id, ":%s", pn(node)->name);
      iter_nodes_attr(pn, decl_vars);
      iter_nodes_attr(pn, decl_funcs);
      print(pn(node)->stmt, ngap);
      break;
    case Function:
      print_node(func, id, ":%s", fn(node)->name);
      iter_nodes_attr(fn, param);
      iter_nodes_attr(fn, decl_vars);
      iter_nodes_attr(fn, decl_funcs);
      print(fn(node)->stmt, ngap);
      break;
    case Decl:
      print_node(decl, id, ":%s", type2str(NULL, dn(node)->type));
      iter_nodes_attr(dn, vars);
      break;
    case CompStmt:
      print_node(comp, id, "");
      iter_nodes_attr(cn, stmts);
      break;
    case BinExpr:
      print_node(binary, id, ":%s",
          binOpLit[bn(node)->op]);
      print(bn(node)->lval, ngap);
      print(bn(node)->rval, ngap);
      break;
    case UnaExpr:
      print_node(unary, id, ":%s",
          unaOpLit[un(node)->op]);
      print(un(node)->val, ngap);
      break;
    case Assign:
      print_node(assignment, id, "");
      print(an(node)->lval, ngap);
      print(an(node)->rval, ngap);
      break;
    case Var:
      print_node(var, id, ":%s", vrn(node)->name);
      break;
    case VarRef:
      print_node(ref, id, ":%s", vrn(node)->name);
      iter_nodes_attr(vrn, index);
      break;
    case Val:
      print_node(val, id, ":%s", val2str(vln(node)));
      break;
    case Invocation:
      print_node(invocation, id, ":%s",
          ivn(node)->name);
      iter_nodes_attr(ivn, args);
      break;
    case IfTest:
      print_node(if, id, "");
      print(ifn(node)->expr, ngap);
      print(ifn(node)->then, ngap);
      print(ifn(node)->els, ngap);
      break;
    case While:
      print_node(while, id, "");
      print(wn(node)->expr, ngap);
      print(wn(node)->stmt, ngap);
      break;
    default:
      break;
  }
}
