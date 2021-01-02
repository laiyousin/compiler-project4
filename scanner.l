%{
/*
 * scanner.l reference
 */

#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "parser.h"

int fileno(FILE *);

#define YY_USER_ACTION \
    yylloc.first_line = line_no; \
    yylloc.first_column = col_no; \
    col_no += yyleng;

#define RETURN(FUNC, TOKEN) { \
  {FUNC;} return TOKEN; \
}

#define MAX_LINE_LENG      256
#define LIST       strcat(buffer, yytext)
#define LIST_FLUSH do{ if(opt_list) printf("%s", buffer); *buffer = 0; }while(0)
#define LOG(TYPE) do{ LIST; if(opt_token) fprintf(stderr, "token(type:%-10s) on line %4d, %3d : %s\n", #TYPE, line_no, col_no - yyleng + 1, yytext); } while(0)


#ifdef __cplusplus
extern "C" int yylex(void);
#endif

int line_no = 1;
int col_no = 0;
char buffer[MAX_LINE_LENG];
int opt_list = 1;
int opt_token = 0;

%}

%option nounput
%option noinput

A [aA]
B [bB]
C [cC]
D [dD]
E [eE]
F [fF]
G [gG]
H [hH]
I [iI]
J [jJ]
K [kK]
L [lL]
M [mM]
N [nN]
O [oO]
P [pP]
Q [qQ]
R [rR]
S [sS]
T [tT]
U [uU]
V [vV]
W [wW]
X [xX]
Y [yY]
Z [zZ]
BLANKS [ \t]*

DIGIT [0-9]

%x COMMENT

%%

{P}{R}{O}{G}{R}{A}{M}        RETURN(LOG(KEYWORD), PROGRAM);
{V}{A}{R}                    RETURN(LOG(KEYWORD), VAR);
{A}{R}{R}{A}{Y}              RETURN(LOG(KEYWORD), ARRAY);
{O}{F}                       RETURN(LOG(KEYWORD), OF);
{I}{N}{T}{E}{G}{E}{R}        RETURN(LOG(KEYWORD), INTEGER);
{R}{E}{A}{L}                 RETURN(LOG(KEYWORD), REAL);
{S}{T}{R}{I}{N}{G}           RETURN(LOG(KEYWORD), STRING);
{F}{U}{N}{C}{T}{I}{O}{N}     RETURN(LOG(KEYWORD), FUNCTION);
{P}{R}{O}{C}{E}{D}{U}{R}{E}  RETURN(LOG(KEYWORD), PROCEDURE);
{B}{E}{G}{I}{N}              RETURN(LOG(KEYWORD), PBEGIN);
{E}{N}{D}                    RETURN(LOG(KEYWORD), END);
{I}{F}                       RETURN(LOG(KEYWORD), IF);
{T}{H}{E}{N}                 RETURN(LOG(KEYWORD), THEN);
{E}{L}{S}{E}                 RETURN(LOG(KEYWORD), ELSE);
{W}{H}{I}{L}{E}              RETURN(LOG(KEYWORD), WHILE);
{D}{O}                       RETURN(LOG(KEYWORD), DO);
{N}{O}{T}                    RETURN(LOG(KEYWORD), NOT);
{A}{N}{D}                    RETURN(LOG(KEYWORD), AND);
{O}{R}                       RETURN(LOG(KEYWORD), OR);

"("                          RETURN(LOG(KEYWORD), LPAREN);
")"                          RETURN(LOG(KEYWORD), RPAREN);
";"                          RETURN(LOG(KEYWORD), SEMICOLON);
"."                          RETURN(LOG(KEYWORD), DOT);
","                          RETURN(LOG(KEYWORD), COMMA);
":"                          RETURN(LOG(KEYWORD), COLON);
"["                          RETURN(LOG(KEYWORD), LBRACE);
"]"                          RETURN(LOG(KEYWORD), RBRACE);
".."                         RETURN(LOG(KEYWORD), DOTDOT);
":="                         RETURN(LOG(KEYWORD), ASSIGNMENT);
"+"                          RETURN(LOG(KEYWORD), ADDOP);
"-"                          RETURN(LOG(KEYWORD), SUBOP);
"*"                          RETURN(LOG(KEYWORD), MULOP);
"/"                          RETURN(LOG(KEYWORD), DIVOP);
">"                          RETURN(LOG(KEYWORD), GTOP);
"<"                          RETURN(LOG(KEYWORD), LTOP);
"="                          RETURN(LOG(KEYWORD), EQOP);
">="                         RETURN(LOG(KEYWORD), GETOP);
"<="                         RETURN(LOG(KEYWORD), LETOP);
"!="                         RETURN(LOG(KEYWORD), NEQOP);

    /*[a-z]([a-zA-Z0-9_])*_    fprintf(stderr, "[ERROR] line %4d:%3d bad IDENTIFIER %s\n", line_no, col_no-(int)yyleng+1, yytext);*/

[a-z]([a-zA-Z0-9_])*[a-zA-Z0-9]|[a-z]   RETURN(LOG(IDENTIFIER); yylval.text = strdup(yytext), IDENTIFIER);

{DIGIT}+                                    RETURN(LOG(NUMBER); yylval.val = strtol(yytext, NULL, 10), INTEGERNUM);// integer
{DIGIT}+"."{DIGIT}+                         RETURN(LOG(NUMBER); yylval.rval = strtod(yytext, NULL);, REALNUMBER); // real
{DIGIT}+{E}[+-]?{DIGIT}+                    RETURN(LOG(NUMBER); yylval.rval = ator(yytext), SCIENTIFIC); // scientific
{DIGIT}+"."{DIGIT}+{E}[+-]?{DIGIT}+         RETURN(LOG(NUMBER); yylval.rval = ator(yytext), SCIENTIFIC); // scientific

\"([^\n\"]|\\\")*\" {
  LOG(STRING);
  col_no = col_no - yyleng;
  for(size_t c = 0; c < strlen(yytext); c++){
    if (yytext[c] == '\n'){
      line_no++, col_no = 0;
    }
    else col_no++;
  }
  yylval.text = strdup(yytext);
  return(LITERALSTR);
}

"//"[^\n]* {
  LIST;
  if(opt_token) fprintf(stderr, "[INFO ] line %4d:%3d comment string\n", line_no, col_no-(int)yyleng+1);
}

"/*" {
  LIST;
  if(opt_token) fprintf(stderr, "[INFO ] line %4d:%3d comment string start\n", line_no, col_no-(int)yyleng+1);
  BEGIN(COMMENT);
}

<COMMENT>"*/" {
  LIST;
  BEGIN(INITIAL);
  if(opt_token) fprintf(stderr, "[INFO ] line %4d:%3d comment string end\n", line_no, col_no-(int)yyleng+1);
}

<COMMENT>\n {
  LIST;
  LIST_FLUSH;
  line_no++, col_no = 0;
}

<COMMENT>. { LIST; }

\n {
  LIST;
  LIST_FLUSH;
  line_no++, col_no = 0;
}

[ \t\f\r]         LIST;

#[ \t]*{P}{R}{A}{G}{M}{A}[ \t]+{L}{I}{S}{T}[ \t]+{O}{N}    { LIST; opt_list = 1; }
#[ \t]*{P}{R}{A}{G}{M}{A}[ \t]+{L}{I}{S}{T}[ \t]+{O}{F}{F} { LIST; opt_list = 0; }

.  { LIST; fprintf(stderr, "[ERROR] line %4d:%3d lexical analyzer error %s\n", line_no, col_no-(int)yyleng+1, yytext), exit(1); }
