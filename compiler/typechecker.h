#ifndef typechecker_h
#define typechecker_h

#include "ast.h"
#include "common.h"
#include "symbols.h"

bool typechecker_run(struct ast_node *root, symbol_table_t *table);

#endif
