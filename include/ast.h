#ifndef __AST_H__
#define __AST_H__

#include "loc.h"
#include "util.h"
#include <stdint.h>

#define REAL_AS_DOUBLE 0

#if REAL_AS_DOUBLE
  #define real    double
  #define ator    atof
  #define RFMT    "%lf"
  #define REALFMT "D"
  #define realfmt "d"
  #define ldc_real "ldc2_w"
  #define REAL_SIZE 2
#else
  #define real    float
  #define ator    (float)atof
  #define RFMT    "%f"
  #define REALFMT "F"
  #define realfmt "f"
  #define ldc_real "ldc"
  #define REAL_SIZE 1
#endif


#define NodeTypes           \
  Prog, Decl, CompStmt,     \
  BinExpr, UnaExpr,         \
  Assign, Var, VarRef, Val, \
  Function, Invocation,     \
  IfTest, While

typedef enum{ NodeTypes } NodeType;

typedef struct TypeTag *Type, TypeStr;

typedef struct NodeTag{
  NodeType nt;
  LocType  loc;
  Type   type;
} *Node, NodeStr;

#define Obj void*

typedef struct ConsTag{
  Obj car;
  struct ConsTag *cdr;
} *Cons, ConsStr;

#define DECCONS(T, ...) \
  T new ## T(DECL_TYPE(__VA_ARGS__))

#define DECNODE(NT, FILEDS) \
  typedef struct{ \
    NodeType nt; \
    LocType  loc; \
    Type   type; \
    FILEDS; \
  } *NT ## Node, NT ## NodeStr

#define DEFNODE(T, ...) \
  DECNODE(T, \
    SEMI_TYPE(__VA_ARGS__) \
  ); \
  DECCONS(T ## Node, LocType, loc, __VA_ARGS__)

extern char const *typeLit[];
extern char const *unaOpLit[];
extern char const *binOpLit[];
extern char const *valfmt[];

typedef enum{
  ARRAY_TYPE,
  REAL_TYPE,
  INTEGER_TYPE,
  STRING_TYPE,
} Type_t;

typedef enum {
  Neg, Not,
} UnaOp;


typedef enum {
  Add, Sub,
  Div, Mul,
  And, Or,
  Lt, Gt, Eq,
  Let, Get, Neq
} BinOp;


union DataU{
  int    ival;
  real   rval;
  char * text;
#ifdef __cplusplus
  DataU() = default;
  DataU(int &i):ival(i){};
  DataU(double &d):rval(d){};
  DataU(char *&t):text(t){};
#endif
};

typedef union DataU Data;

DECCONS(Cons,
  Obj, car,
  Cons, cdr
);

DEFNODE(While,
  Node, expr,
  Node, stmt
);

DEFNODE(IfTest,
  Node, expr,
  Node, then,
  Node, els
);

DEFNODE(Invocation,
  char*, name,
  Cons , args
);

DEFNODE(Val,
  Data, data
);

DEFNODE(Var,
  char*, name
);

DEFNODE(VarRef,
  char*, name,
  Cons , index
);

DEFNODE(Assign,
  Node, lval,
  Node, rval
);

DEFNODE(UnaExpr,
  UnaOp, op,
  Node , val
);

DEFNODE(BinExpr,
  BinOp, op,
  Node, lval,
  Node, rval
);

DEFNODE(CompStmt,
  Cons, stmts
);

DEFNODE(Decl,
  Cons, vars
);

DEFNODE(Function,
  char*, name,
  Cons , param,
  Cons , decl_vars,
  Cons , decl_funcs,
  Node, stmt
);

DEFNODE(Prog,
  char*, name,
  Cons , idents,
  Cons , decl_vars,
  Cons , decl_funcs,
  Node, stmt
);

struct TypeTag{
  Type_t t;
  ValNode beg, end;
  struct TypeTag *next;
};

DECCONS(Type,
  Type_t, t,
  ValNode, beg,
  ValNode, end,
  Type, next
);

const char *val2str(ValNode v);
const char *type2str(char *b, Type t);
char *params2str(Cons dl, char *buffer);
char *funcOvar2str(Type rtn, Cons dl, char *buffer);

#define _DEFCONS(T, extra, ...) \
  T new ## T(DECL_TYPE(__VA_ARGS__)){ \
    T e = (T)malloc(sizeof(T ## Str)); \
    extra; INIT_TYPE(__VA_ARGS__); \
    return e; \
  }

#define DEFCONS(T, ...) \
  _DEFCONS(T, , __VA_ARGS__)

#define DEFNODECONS(T, ...) \
  _DEFCONS(T ## Node, e->nt = T; , LocType, loc, __VA_ARGS__);

int same_type(Type a, Type b);
Node set_type(Obj n, Type t);

extern const char *builtins[];

extern Type builtinsT[];

int isReserved(const char *name);

Type reserveType(char *name);

#endif
