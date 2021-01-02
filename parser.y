%{
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "loc.h"
#include "ast.h"
#include "ast-print.h"
#include "ast-dump.h"
#include "ast-visit.h"
#include "ast-dumper.h"
#include "ast-symtabctor.h"
#include "ast-semanlzer.h"
#include "ast-codegen.h"
#include "hash.h"
#include "error.h"

#define YYLTYPE LocType

#define MAX_LINE_LENG      256
extern int line_no, col_no, opt_list;
extern char buffer[MAX_LINE_LENG];
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */
extern int yyleng;

int pass_error = 0;
extern char *output = NULL;
static Node root = NULL;


extern
#ifdef __cplusplus
"C"
#endif
int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);

%}

%locations

%token PROGRAM VAR ARRAY OF INTEGER REAL STRING FUNCTION PROCEDURE PBEGIN END IF THEN ELSE WHILE DO NOT AND OR

%token LPAREN RPAREN SEMICOLON DOT COMMA COLON LBRACE RBRACE DOTDOT ASSIGNMENT ADDOP SUBOP MULOP DIVOP LTOP GTOP EQOP GETOP LETOP NEQOP

%token IDENTIFIER REALNUMBER INTEGERNUM SCIENTIFIC LITERALSTR

%union {
  int val;
  char* text;
  real rval;
  Node node;
  Cons cons;
  Type type;
  BinOp binop;
  ProgNode prog;
  FunctionNode func;
  DeclNode decl;
  CompStmtNode comp;
  BinExprNode bine;
  UnaExprNode unae;
  AssignNode assn;
  VarRefNode vref;
  ValNode valn;
  InvocationNode invo;
  IfTestNode iftn;
  WhileNode whil;
}

%type <prog> prog
%type <func> subprogram_declaration subprogram_head
%type <comp> compound_statement
%type <valn> num
%type <vref> variable
%type <invo> procedure_statement
%type <node> statement expression
%type <node> boolexpression simple_expression term factor
%type <cons> identifier_list declarations subprogram_declarations
%type <cons> expression_list arguments tail parameter_list statement_list
%type <type> type standard_type
%type <binop> addop mulop relop
%type <text> IDENTIFIER

%%

prog : PROGRAM IDENTIFIER LPAREN identifier_list RPAREN SEMICOLON
     declarations
     subprogram_declarations
     compound_statement
     DOT { root = (Node)newProgNode(@1, $2, $4, $7, $8, (Node)$9); }
     ;

identifier_list : IDENTIFIER
                { $$ = newCons(newVarNode(@1, $1), NULL); }
                | IDENTIFIER COMMA identifier_list
                { $$ = newCons(newVarNode(@1, $1), $3); }
                ;

declarations : VAR identifier_list COLON type SEMICOLON declarations
             { $$ = newCons(set_type((Node)newDeclNode(@1, $2), $4), $6); }
             | { $$ = NULL; }
             ;

type : standard_type { $$ = $1; }
     | ARRAY LBRACE num DOTDOT num RBRACE OF type
     { $$ = newType(ARRAY_TYPE, $3, $5, $8); }
     ;

standard_type : INTEGER { $$ = newType(INTEGER_TYPE, NULL, NULL, NULL); }
              | REAL    { $$ = newType(REAL_TYPE,  NULL, NULL, NULL); }
              | STRING  { $$ = newType(STRING_TYPE,  NULL, NULL, NULL); }
              ;

subprogram_declarations : subprogram_declaration SEMICOLON subprogram_declarations
                        { $$ =  newCons($1, $3); }
                        | { $$ = NULL; }
                        ;

subprogram_declaration : subprogram_head
                       declarations
                       subprogram_declarations
                       compound_statement
                       {
                          $1->decl_vars = $2;
                          $1->decl_funcs = $3;
                          $1->stmt = (Node)$4;
                          $$ = $1;
                       }
                       ;

subprogram_head : FUNCTION IDENTIFIER arguments COLON standard_type SEMICOLON
                { $$ = (FunctionNode)set_type(newFunctionNode(@1, $2, $3, NULL, NULL, NULL), $5); }
                | PROCEDURE IDENTIFIER arguments SEMICOLON
                { $$ = newFunctionNode(@1, $2, $3, NULL, NULL, NULL); }
                ;

arguments : LPAREN parameter_list RPAREN { $$ = $2; }
          | { $$ = NULL; }
          ;

parameter_list : optional_var identifier_list COLON type
               { $$ = newCons(set_type((Node)newDeclNode(@1, $2), $4), NULL); }
               | optional_var identifier_list COLON type SEMICOLON parameter_list
               { $$ = newCons(set_type((Node)newDeclNode(@1, $2), $4), $6);}
               ;

optional_var : VAR
             |
             ;

compound_statement : PBEGIN statement_list END { $$ = newCompStmtNode(@1, $2); }

statement_list : statement { $$ = newCons($1, NULL); }
               | statement SEMICOLON statement_list { $$ = newCons($1, $3); }
               ;

statement : variable ASSIGNMENT expression
          { $$ = (Node)newAssignNode(@1, (Node)$1, $3); }
          | procedure_statement
          { $$ = (Node)$1; }
          | compound_statement
          { $$ = (Node)$1; }
          | IF expression THEN statement ELSE statement
          { $$ = (Node)newIfTestNode(@1, $2, $4, $6); }
          | WHILE expression DO statement
          { $$ = (Node)newWhileNode(@1, $2, $4); }
          | { $$ = NULL; }
          ;

variable : IDENTIFIER tail { $$ = newVarRefNode(@1, $1, $2); }

tail : LBRACE expression RBRACE tail { $$ = newCons($2, $4); }
     | { $$ = NULL; }
     ;

procedure_statement : IDENTIFIER
                    { $$ = newInvocationNode(@1, $1, NULL); }
                    | IDENTIFIER LPAREN expression_list RPAREN
                    { $$ = newInvocationNode(@1, $1, $3); }
                    ;

expression_list : expression
                { $$ = newCons($1, NULL); }
                | expression COMMA expression_list
                { $$ = newCons($1, $3); }
                ;

expression : boolexpression
           { $$ = (Node)$1; }
           | boolexpression AND boolexpression
           { $$ = (Node)newBinExprNode(@2, And, $1, $3); }
           | boolexpression OR boolexpression
           { $$ = (Node)newBinExprNode(@2, Or, $1, $3); }
           ;


boolexpression : simple_expression
               { $$ = $1; }
               | simple_expression relop simple_expression
               { $$ = (Node)newBinExprNode(@2, $2, $1, $3); }
               ;

simple_expression : term
                  { $$ = $1; }
                  | simple_expression addop term
                  { $$ = (Node)newBinExprNode(@2, $2, $1, $3); }
                  ;

term : factor
     { $$ = $1; }
     | term mulop factor
     { $$ = (Node)newBinExprNode(@2, $2, $1, $3); }
     ;

factor : variable
       { $$ = (Node)$1; }
       | IDENTIFIER LPAREN expression_list RPAREN
       { $$ = (Node)newInvocationNode(@1, $1, $3); }
       | num
       { $$ = (Node)$1; }
       | LITERALSTR
       { $$ = (Node)set_type(
                (Node)newValNode(@1, (Data)yylval.text),
                newType(STRING_TYPE, NULL, NULL, NULL)); }
       | LPAREN expression RPAREN
       { $$ = (Node)$2; }
       | NOT factor
       { $$ = (Node)newUnaExprNode(@1, Not, $2); }
       | SUBOP factor
       { $$ = (Node)newUnaExprNode(@1, Neg, $2); }
       ;

addop : ADDOP { $$ = Add; }
      | SUBOP { $$ = Sub; }
      ;

mulop : MULOP { $$ = Mul; }
      | DIVOP { $$ = Div; }
      ;

relop : LTOP  { $$ = Lt; }
      | GTOP  { $$ = Gt; }
      | EQOP  { $$ = Eq; }
      | LETOP { $$ = Let; }
      | GETOP { $$ = Get; }
      | NEQOP { $$ = Neq; }
      ;

num : INTEGERNUM { $$ = (ValNode)set_type((Node)newValNode(@1, (Data)yylval.val), newType(INTEGER_TYPE, NULL, NULL, NULL)); }
    | REALNUMBER { $$ = (ValNode)set_type((Node)newValNode(@1, (Data)yylval.rval), newType(REAL_TYPE, NULL, NULL, NULL)); }
    | SCIENTIFIC { $$ = (ValNode)set_type((Node)newValNode(@1, (Data)yylval.rval), newType(REAL_TYPE, NULL, NULL, NULL)); }
    ;

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "[ERROR] line %4d:%3d %s, Unmatched token: %s\n",
            line_no, col_no-(int)yyleng+1, buffer, yytext);
    pass_error = 1;
}

int main(int argc, char *argv[]) {

    char c;
    while((c=getopt(argc, argv, "o:")) != -1){
      switch(c){
        case 'o':
          output = optarg;
          break;
        case '?':
            fprintf(stderr, "Illegal option:-%c\n", isprint(optopt)?optopt:'#');
            break;
        default:
            fprintf( stderr, "Usage: %s [-o output] filename\n", argv[0]), exit(0);
            break;
      }
    }

    FILE *fp = argc == 1 ? stdin : fopen(argv[optind], "r");

    if(fp == NULL)
        fprintf( stderr, "Open file error\n" ), exit(-1);

    yyin = fp;
    yyparse();
    if(!pass_error && root){
      //visit(root, dumper);
      Hash symtab = SymbolTable(root);
      if(pass_error) return 0;
      SemanticCheck(root, symtab);
      if(pass_error) return 0;
      CodeGen(root, symtab);
    }
    return 0;
}
