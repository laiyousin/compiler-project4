#include "ast.h"
#include "cast.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char const *valfmt[] = {"%p", "%lf", "%d", "%s"};

char const *unaOpLit[] = { "-", "not" };
char const *binOpLit[] = {
  "+", "-", "/", "*", "&&", "||",
  "<", ">", "==", "<=", ">=", "!="
};
char const *typeLit[] = { "array of", "real", "int", "str" };

DEFCONS(Type,
  Type_t, t,
  ValNode, beg,
  ValNode, end,
  Type, next
);

DEFCONS(Cons,
  Obj, car,
  Cons, cdr
);

DEFNODECONS(While,
  Node, expr,
  Node, stmt
);

DEFNODECONS(IfTest,
  Node, expr,
  Node, then,
  Node, els
);

DEFNODECONS(Invocation,
  char*, name,
  Cons , args
);

DEFNODECONS(Val,
  Data, data
);

DEFNODECONS(Var,
  char*, name
);

DEFNODECONS(VarRef,
  char*, name,
  Cons , index
);

DEFNODECONS(Assign,
  Node, lval,
  Node , rval
);

DEFNODECONS(UnaExpr,
  UnaOp, op,
  Node, val
);

DEFNODECONS(BinExpr,
  BinOp, op,
  Node, lval,
  Node, rval
);

DEFNODECONS(CompStmt,
  Cons, stmts
);

DEFNODECONS(Decl,
  Cons, vars
);

DEFNODECONS(Function,
  char*, name,
  Cons , param,
  Cons , decl_vars,
  Cons , decl_funcs,
  Node, stmt
);


DEFNODECONS(Prog,
  char*, name,
  Cons , idents,
  Cons , decl_vars,
  Cons , decl_funcs,
  Node, stmt
);

const char *val2str(ValNode v){
  static char buffer[32];
  if(!v || !v->type) return "null";
  if(v->type->t == STRING_TYPE)
    return v->data.text;
  sprintf(buffer, valfmt[v->type->t], v->data);
  return buffer;
}

const char *type2str(char *b, Type t){
  static char buffer[128];
  if(!b) b = buffer;
  if(!t){
    sprintf(b, "void");
    return b;
  }
  *b = 0;
  switch(t->t){
    case ARRAY_TYPE:
      type2str(b, t->next);
      sprintf(b + strlen(b), "[%d~%d]",
          t->beg->data.ival,
          t->end->data.ival);
      break;
    case REAL_TYPE:
    case INTEGER_TYPE:
    case STRING_TYPE:
      sprintf(b, "%s", typeLit[t->t]);
      break;
    default:
      fprintf(stderr, "unsupported type %d\n", t->t);
      exit(0);
  }
  return b;
}

int same_type(Type a, Type b){
  if(!a && !b) return 1;
  if(!(a && b)) return 0;
  if(a->t == b->t && a->t == ARRAY_TYPE)
    return same_type(a->next, b->next);
  return a->t == b->t;
}

Node set_type(Obj n, Type t){
  ((Node)n)->type = t;
  if(((Node)n)->nt == Decl)
    for(Cons vars = dn(n)->vars; vars; vars = vars->cdr)
      set_type(vars->car, t);
  return (Node)n;
}

char *params2str(Cons dl, char *buffer){
  int first = 1;
  *buffer = 0;
  sprintf(buffer, "(");
  for(Cons d = dl; d; d = d->cdr){
    for(Cons v = dn(d->car)->vars; v; v = v->cdr){
      if(first) first = 0;
      else sprintf(buffer + strlen(buffer), ", ");
      type2str(buffer + strlen(buffer), vn(v->car)->type);
    }
  }
  sprintf(buffer + strlen(buffer), ")");
  return buffer;
}

char *funcOvar2str(Type rtn, Cons dl, char *buffer){
  *buffer = 0;
  type2str(buffer, rtn);
  if(dl){
    char *p = buffer + strlen(buffer);
    *p++ = ' ', *p = 0;
    params2str(dl, p);
  }
  return buffer;
}

const char *builtins[] = {"readlnI", "readlnR", "writelnI", "writelnR", "writelnS"};

TypeStr TypeI = {
  .t = INTEGER_TYPE,
  .beg = NULL,
  .end = NULL,
  .next = NULL
};

TypeStr TypeF = {
  .t = REAL_TYPE,
  .beg = NULL,
  .end = NULL,
  .next = NULL
};

Type builtinsT[] = {&TypeI, &TypeF, NULL, NULL, NULL};

int isReserved(const char *name){
  for(int i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++)
    if(!strcmp(name, builtins[i])) return 1;
  return 0;
}

Type reserveType(char *name){
  for(int i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++)
    if(!strcmp(name, builtins[i])) return builtinsT[i];
  return NULL;
}
