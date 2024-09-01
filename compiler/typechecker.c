#include "typechecker.h"
#include "ast.h"
#include "common.h"
#include "scanner.h"
#include "symbols.h"

static const char *op_to_string(enum scanner_token_type op) {
  switch (op) {
  case TOKEN_PLUS:
    return "+";
  case TOKEN_MINUS:
    return "-";
  case TOKEN_STAR:
    return "*";
  case TOKEN_SLASH:
    return "/";
  default:
    UNREACHABLE();
    return NULL;
  }
}

static const char *type_to_string(enum ast_data_type type) {
  switch (type) {
  case TYPE_I32:
    return "i32";
  case TYPE_BOOL:
    return "bool";
  case TYPE_ERROR:
    return "error";
  default:
    UNREACHABLE();
    return NULL;
  }
}

enum ast_data_type typecheck(struct ast_node *root, symbol_table_t *table) {
  assert(root);
  assert(table);

  switch (root->type) {
  case AST_PROGRAM:
    for (int i = 0; i < root->as.program.num_statements; i++) {
      if (typecheck(root->as.program.statements[i], table) == TYPE_ERROR)
        return TYPE_ERROR;
    }
    return TYPE_I32; // placeholder

  case AST_PRINT_STMT:
    return typecheck(root->as.print_stmt.expr, table);

  case AST_BINARY_EXPR: {
    enum ast_data_type left = typecheck(root->as.binary_expr.left, table);
    enum ast_data_type right = typecheck(root->as.binary_expr.right, table);

    if (left == TYPE_ERROR || right == TYPE_ERROR) {
      return TYPE_ERROR;
    }

    if (left == TYPE_I32 && right == TYPE_I32) {
      return TYPE_I32;
    }

    printf("[error] Type mismatch for binary expression '%s %s %s'\n",
           type_to_string(left), op_to_string(root->as.binary_expr.token.type),
           type_to_string(right));
    return TYPE_ERROR;
  }

  case AST_UNARY_EXPR: {
    enum ast_data_type right = typecheck(root->as.unary_expr.right, table);

    if (right == TYPE_ERROR)
      return TYPE_ERROR;

    if (right == TYPE_I32)
      return TYPE_I32;

    printf("[error] Type mismatch for unary expression '%s %s'\n",
           op_to_string(root->as.unary_expr.token.type), type_to_string(right));
    return TYPE_ERROR;
  }

  case AST_GROUPING_EXPR:
    return typecheck(root->as.grouping_expr.expr, table);

  case AST_IDENTIFIER_EXPR:
    return symbol_table_get(table, &root->as.idenitifer)->as.variable_decl.type;

  case AST_VARIABLE_DECL: {
    enum ast_data_type initialiser =
        typecheck(root->as.variable_decl.initialiser, table);

    if (initialiser == TYPE_ERROR)
      return TYPE_ERROR;

    if (root->as.variable_decl.type == initialiser)
      return initialiser;

    printf("[error] Cannot assign %s to variable '%.*s' of type %s.\n",
           type_to_string(initialiser), root->as.variable_decl.name.length,
           root->as.variable_decl.name.start,
           type_to_string(root->as.variable_decl.type));
    return TYPE_ERROR;
  }

  case AST_LITERAL_EXPR:
    return root->as.literal_expr.type;

  default:
    UNREACHABLE();
    return false;
  }
}
