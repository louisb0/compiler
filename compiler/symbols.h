#ifndef symbols_h
#define symbols_h

#include "ast.h"
#include "scanner.h"

typedef struct symbol_table_t symbol_table_t;

struct symbol_entry {
  struct scanner_token *name;
  struct ast_node *decl;

  struct symbol_entry *next;
};

symbol_table_t *symbol_table_new(int capacity);
void symbol_table_free(symbol_table_t **table);

void symbol_table_add(symbol_table_t *table, struct scanner_token *name,
                      struct ast_node *decl);
struct ast_node *symbol_table_get(symbol_table_t *table,
                                  struct scanner_token *name);

#endif
