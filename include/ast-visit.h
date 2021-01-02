#ifndef __AST_VISIT_H__
#define __AST_VISIT_H__

#include "ast.h"
#include "pass.h"
#include "cast.h"

typedef void (*Visitor)(Node);

PASS_DEC(void, visit, Visitor);
PASS_DEC(void, visitChildren, Visitor);

#define visitChildren(n, ...) visitChildren(nd(n), ##__VA_ARGS__)

#endif
