#ifndef __AST_SEMANLZER_H__
#define __AST_SEMANLZER_H__

#include "ast.h"
#include "pass.h"
#include "hash.h"

PASS_DEC(void, semanlzer);
void SemanticCheck(Node node, Hash tab);

#endif
