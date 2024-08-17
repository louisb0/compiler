#include "resolver.h"
#include "ast.h"
#include "common.h"
#include "symbols.h"

void resolver_generate_table(struct ast_node *root, symbol_table_t *table) {
  assert(root);
  assert(table);

  switch (root->type) {
  case AST_PROGRAM:
    for (int i = 0; i < root->as.program.num_statements; i++) {
      resolver_generate_table(root->as.program.statements[i], table);
    }
    break;

  case AST_VARIABLE_DECL: {
    symbol_table_add(table, &root->as.variable_decl.name, root);
    break;
  }

  case AST_PRINT_STMT:
    resolver_generate_table(root->as.print_stmt.expr, table);
    break;

  case AST_BINARY_EXPR:
    resolver_generate_table(root->as.binary_expr.left, table);
    resolver_generate_table(root->as.binary_expr.right, table);
    break;

  case AST_UNARY_EXPR:
    resolver_generate_table(root->as.unary_expr.right, table);
    break;

  case AST_GROUPING_EXPR:
    resolver_generate_table(root->as.grouping_expr.expr, table);
    break;

  case AST_LITERAL_EXPR:
  case AST_IDENTIFIER_EXPR:
    break;

  default:
    UNREACHABLE();
    break;
  }
}
