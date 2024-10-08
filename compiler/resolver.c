#include "resolver.h"
#include "ast.h"
#include "common.h"
#include "symbols.h"

bool resolver_generate_table(struct ast_node *root, symbol_table_t *table) {
  assert(root);
  assert(table);

  switch (root->type) {
  case AST_PROGRAM:
    for (int i = 0; i < root->as.program.num_statements; i++) {
      if (!resolver_generate_table(root->as.program.statements[i], table))
        return false;
    }
    return true;

  case AST_PRINT_STMT:
    return resolver_generate_table(root->as.print_stmt.expr, table);

  case AST_BINARY_EXPR: {
    return resolver_generate_table(root->as.binary_expr.left, table) &&
           resolver_generate_table(root->as.binary_expr.right, table);
  }

  case AST_UNARY_EXPR:
    return resolver_generate_table(root->as.unary_expr.right, table);

  case AST_GROUPING_EXPR:
    return resolver_generate_table(root->as.grouping_expr.expr, table);

  case AST_ASSIGNMENT_STMT:
    if (!symbol_table_get(
            table, &root->as.assignment_stmt.identifier->as.identifier)) {
      printf("[error] Identifier '%.*s' is undeclared at time of reference.\n",
             root->as.assignment_stmt.identifier->as.identifier.length,
             root->as.assignment_stmt.identifier->as.identifier.start);
      return false;
    }
    return true;

  case AST_IDENTIFIER_EXPR:
    if (!symbol_table_get(table, &root->as.identifier)) {
      printf("[error] Identifier '%.*s' is undeclared at time of reference.\n",
             root->as.identifier.length, root->as.identifier.start);
      return false;
    }
    return true;

  case AST_VARIABLE_DECL: {
    if (symbol_table_get(table, &root->as.variable_decl.name)) {
      printf("[error] Cannot redeclare variable '%.*s'.\n",
             root->as.identifier.length, root->as.identifier.start);
      return false;
    }

    symbol_table_add(table, &root->as.variable_decl.name, root);
    return true;
  }

  case AST_LITERAL_EXPR:
    return true;

  default:
    UNREACHABLE();
    return false;
  }
}
