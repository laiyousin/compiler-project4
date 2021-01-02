#ifndef __AST_SYMTABCTOR_H__
#define __AST_SYMTABCTOR_H__

#include "ast.h"
#include "pass.h"
#include "hash.h"

PASS_DEC(void, symtabctor);
Hash SymbolTable(Node node);

#endif
