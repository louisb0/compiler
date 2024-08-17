#include "typechecker.h"
#include "ast.h"
#include "common.h"
#include "symbols.h"

static enum ast_data_type typechecker_type(struct ast_node *node,
                                           symbol_table_t *table) {
  switch (node->type) {
  case AST_LITERAL_EXPR:
    return node->as.literal_expr.type;

  case AST_IDENTIFIER_EXPR: {
    struct ast_node *symbol = symbol_table_get(table, &node->as.idenitifer);
    if (symbol == NULL) {
      fprintf(stderr, "Error: Undeclared identifier\n");
      return TYPE_I32; // TODO: dont cascade
    }
    return symbol->as.variable_decl.type;
  }

  case AST_BINARY_EXPR: {
    enum ast_data_type left =
        typechecker_type(node->as.binary_expr.left, table);
    enum ast_data_type right =
        typechecker_type(node->as.binary_expr.right, table);

    if (left != right) {
      fprintf(stderr, "Error: Type mismatch in binary expression\n");
      return TYPE_I32; // TODO: dont cascade
    }

    return left;
  }

  case AST_UNARY_EXPR:
    return typechecker_type(node->as.unary_expr.right, table);

  case AST_GROUPING_EXPR:
    return typechecker_type(node->as.grouping_expr.expr, table);
  default:
    fprintf(stderr, "Error: Unexpected node type in typechecker_type\n");
    return TYPE_I32;
  }
}

bool typechecker_run(struct ast_node *root, symbol_table_t *table) {
  assert(root);
  assert(table);

  switch (root->type) {
  case AST_PROGRAM:
    for (int i = 0; i < root->as.program.num_statements; i++) {
      if (!typechecker_run(root->as.program.statements[i], table)) {
        return false;
      }
    }
    return true;

  case AST_VARIABLE_DECL: {
    enum ast_data_type initialiser_type =
        typechecker_type(root->as.variable_decl.initialiser, table);

    if (initialiser_type != root->as.variable_decl.type) {
      fprintf(stderr, "Error: Type mismatch in variable declaration\n");
      return false;
    }

    return true;
  }

  case AST_PRINT_STMT:
    return typechecker_run(root->as.print_stmt.expr, table);

  case AST_BINARY_EXPR: {
    bool left_valid = typechecker_run(root->as.binary_expr.left, table);
    bool right_valid = typechecker_run(root->as.binary_expr.right, table);
    if (!left_valid || !right_valid) {
      return false;
    }

    enum ast_data_type left_type =
        typechecker_type(root->as.binary_expr.left, table);
    enum ast_data_type right_type =
        typechecker_type(root->as.binary_expr.right, table);

    if (left_type != right_type) {
      fprintf(stderr, "Error: Type mismatch in binary expression\n");
      return false;
    }

    switch (root->as.binary_expr.token.type) {
    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_STAR:
    case TOKEN_SLASH:
      if (left_type != TYPE_I32) {
        fprintf(stderr,
                "Error: Arithmetic operations only valid for integers\n");
        return false;
      }
      break;
    default:
      UNREACHABLE();
      return false;
    }
    return true;
  }

  case AST_UNARY_EXPR: {
    bool operand_valid = typechecker_run(root->as.unary_expr.right, table);
    if (!operand_valid) {
      return false;
    }

    enum ast_data_type operand_type =
        typechecker_type(root->as.unary_expr.right, table);

    switch (root->as.unary_expr.token.type) {
    case TOKEN_MINUS:
      if (operand_type != TYPE_I32) {
        fprintf(stderr, "Error: Unary minus only valid for integers\n");
        return false;
      }
      break;
    default:
      UNREACHABLE();
      return false;
    }
    return true;
  }

  case AST_GROUPING_EXPR:
    return typechecker_run(root->as.grouping_expr.expr, table);

  case AST_LITERAL_EXPR:
    return true;

  case AST_IDENTIFIER_EXPR: {
    struct ast_node *symbol = symbol_table_get(table, &root->as.idenitifer);
    if (symbol == NULL) {
      fprintf(stderr, "Error: Use of undeclared identifier\n");
      return false;
    }
    return true;
  }

  default:
    fprintf(stderr, "Error: Unexpected node type in typechecker_run\n");
    return false;
  }
}
