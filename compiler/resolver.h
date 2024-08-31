#ifndef resolver_h
#define resolver_h

#include "ast.h"
#include "symbols.h"

bool resolver_generate_table(struct ast_node *root, symbol_table_t *table);

#endif
