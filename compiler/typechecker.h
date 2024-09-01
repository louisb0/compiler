#ifndef typechecker_h
#define typechecker_h

#include "ast.h"
#include "symbols.h"

enum ast_data_type typecheck(struct ast_node *root, symbol_table_t *table);

#endif
