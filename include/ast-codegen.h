#ifndef __AST_CODEGEN_H__
#define __AST_CODEGEN_H__

#include "ast.h"
#include "pass.h"
#include "hash.h"

PASS_DEC(void, codegen);
void CodeGen(Node node, Hash tab);

#endif
