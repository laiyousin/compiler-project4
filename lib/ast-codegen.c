#include "ast.h"
#include "symtab.h"
#include "ast-visit.h"
#include "ast-codegen.h"
#include "ast-print.h"
#include "cast.h"
#include "hash.h"
#include <search.h>
#include <stdio.h>
#include <stdlib.h>

#define LOCALS_SIZE "50"
#define STACK_SIZE "50"

#define attr_iter(node, attr, iter) \
  for(Cons iter = node->attr; iter; iter = iter->cdr)
#define iter_nodes_attr(node, attr, visitor) \
  for(Cons it = node->attr; it; it = it->cdr) \
  visit((Node)it->car, visitor);

#define gen(fmt, ...) fprintf(fd, fmt, ##__VA_ARGS__)

FILE *fd;
char *prog;
extern char *output;
static Hash symtab;

void CodeGen(Node node, Hash tab){
  symtab = tab;
  visit(node, codegen);
}

void genBuiltins(){
  gen(
      ".method public static writelnI(I)V\n"
      ".limit locals 5\n"
      ".limit stack 5\n"
      "    getstatic java/lang/System/out Ljava/io/PrintStream;\n"
      "    iload 0\n"
      "    invokevirtual java/io/PrintStream/println(I)V\n"
      "    return\n"
      ".end method\n"
      "\n"
      ".method public static writelnR(" REALFMT ")V\n"
      "    .limit locals 5\n"
      "    .limit stack 5\n"
      "    getstatic java/lang/System/out Ljava/io/PrintStream;\n"
      "    " realfmt "load 0\n"
      "    invokevirtual java/io/PrintStream/println(" REALFMT ")V\n"
      "    return\n"
      ".end method\n"
      "\n"
      ".method public static writelnS(Ljava/lang/String;)V\n"
      "    .limit locals 5\n"
      "    .limit stack 5\n"
      "    getstatic java/lang/System/out Ljava/io/PrintStream;\n"
      "    aload 0\n"
      "    invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V\n"
      "    return\n"
      ".end method\n"
      "\n"
      ".method public static readlnI()I\n"
      "    .limit locals 10\n"
      "    .limit stack 10\n"
      "    ldc 0\n"
      "    istore 1\n"
      "LAB1:\n"
      "    getstatic java/lang/System/in Ljava/io/InputStream;\n"
      "    invokevirtual java/io/InputStream/read()I\n"
      "    istore 2\n"
      "    iload 2\n"
      "    ldc 10\n"
      "    isub\n"
      "    ifeq LAB2\n"
      "    iload 2\n"
      "    ldc 32\n"
      "    isub\n"
      "    ifeq LAB2\n"
      "    iload 2\n"
      "    ldc 48\n"
      "    isub\n"
      "    ldc 10\n"
      "    iload 1\n"
      "    imul\n"
      "    iadd\n"
      "    istore 1\n"
      "    goto LAB1\n"
      "LAB2:\n"
      "    iload 1\n"
      "    ireturn\n"
      ".end method\n");
}

PASS_DEF(codegen);

PASS_HANDLE(codegen, Node          , node){}

PASS_HANDLE(codegen, ProgNode      , node){
  char fn[128];
  prog = node->name;
  sprintf(fn, "%s.s", node->name);
  fd = fopen(output ? output : fn, "w");
  gen(";gen code into the file\n");
  fclose(fd);
}

PASS_HANDLE(codegen, FunctionNode  , node){
}

PASS_HANDLE(codegen, DeclNode      , node){
}

PASS_HANDLE(codegen, CompStmtNode  , node){
}

PASS_HANDLE(codegen, BinExprNode   , node){
}

PASS_HANDLE(codegen, UnaExprNode   , node){
}

PASS_HANDLE(codegen, AssignNode    , node){
}

PASS_HANDLE(codegen, VarNode       , node){
  /*
  int lv = 0;
  SymEntry se = find_symbol_lv(
      find(symtab, node), node->name, NULL, &lv);
  */
}

PASS_HANDLE(codegen, VarRefNode    , node){
}

PASS_HANDLE(codegen, ValNode       , node){
}

PASS_HANDLE(codegen, InvocationNode, node){
}

PASS_HANDLE(codegen, IfTestNode    , node){
}

PASS_HANDLE(codegen, WhileNode     , node){
}
