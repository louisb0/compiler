#include "ast.h"
#include "common.h"
#include "scanner.h"

struct ast_node *ast_new(enum ast_node_type type) {
  struct ast_node *node = (struct ast_node *)malloc(sizeof(*node));
  if (node == NULL) {
    ERROR_OUT();
  }

  node->type = type;

  return node;
}

void ast_free(struct ast_node **root) {
  assert(root && *root);

  switch ((*root)->type) {
  case AST_PROGRAM:
    for (int i = 0; i < (*root)->as.program.num_statements; i++) {
      ast_free(&(*root)->as.program.statements[i]);
    }
    break;

  case AST_VARIABLE_DECL:
    if ((*root)->as.variable_decl.initialiser) {
      ast_free(&(*root)->as.variable_decl.initialiser);
    }
    break;

  case AST_PRINT_STMT:
    ast_free(&(*root)->as.print_stmt.expr);
    break;

  case AST_BINARY_EXPR:
    ast_free(&(*root)->as.binary_expr.left);
    ast_free(&(*root)->as.binary_expr.right);
    break;

  case AST_UNARY_EXPR:
    ast_free(&(*root)->as.unary_expr.right);
    break;

  case AST_GROUPING_EXPR:
    ast_free(&(*root)->as.grouping_expr.expr);
    break;

  case AST_LITERAL_EXPR:
  case AST_IDENTIFIER_EXPR:
    break;

  default:
    UNREACHABLE();
  }

  free(*root);
  *root = NULL;
}

struct ast_node *ast_new_program(struct ast_node **statements,
                                 int num_statements) {
  if (num_statements > MAX_STATEMENTS) {
    ERROR_OUT();
  }

  struct ast_node *node = ast_new(AST_PROGRAM);
  node->as.program.num_statements = num_statements;

  for (int i = 0; i < num_statements; i++) {
    node->as.program.statements[i] = statements[i];
  }

  return node;
}

struct ast_node *ast_new_variable_decl(struct scanner_token name,
                                       enum ast_data_type type,
                                       bool is_constant,
                                       struct ast_node *initialiser) {
  struct ast_node *node = ast_new(AST_VARIABLE_DECL);
  node->as.variable_decl.name = name;
  node->as.variable_decl.type = type;
  node->as.variable_decl.is_constant = is_constant;
  node->as.variable_decl.initialiser = initialiser;
  return node;
}

struct ast_node *ast_new_print_stmt(struct ast_node *expr) {
  struct ast_node *node = ast_new(AST_PRINT_STMT);
  node->as.print_stmt.expr = expr;
  return node;
}

struct ast_node *ast_new_grouping_expr(struct ast_node *expr) {
  struct ast_node *node = ast_new(AST_GROUPING_EXPR);
  node->as.grouping_expr.expr = expr;
  return node;
}

struct ast_node *ast_new_binary_expr(struct scanner_token token,
                                     struct ast_node *left,
                                     struct ast_node *right) {
  struct ast_node *node = ast_new(AST_BINARY_EXPR);
  node->as.binary_expr.token = token;
  node->as.binary_expr.left = left;
  node->as.binary_expr.right = right;
  return node;
}

struct ast_node *ast_new_unary_expr(struct scanner_token token,
                                    struct ast_node *right) {
  struct ast_node *node = ast_new(AST_UNARY_EXPR);
  node->as.unary_expr.token = token;
  node->as.unary_expr.right = right;
  return node;
}

struct ast_node *ast_new_identifier_expr(struct scanner_token name) {
  struct ast_node *node = ast_new(AST_IDENTIFIER_EXPR);
  node->as.idenitifer = name;
  return node;
}

struct ast_node *ast_new_number_expr(int literal) {
  struct ast_node *node = ast_new(AST_LITERAL_EXPR);
  node->as.literal_expr.type = TYPE_I32;
  node->as.literal_expr.as.i32 = literal;
  return node;
}

struct ast_node *ast_new_bool_expr(bool literal) {
  struct ast_node *node = ast_new(AST_LITERAL_EXPR);
  node->as.literal_expr.type = TYPE_BOOL;
  node->as.literal_expr.as.boolean = literal;
  return node;
}

static const char *ast_data_type_to_string(enum ast_data_type type) {
  switch (type) {
  case TYPE_I32:
    return "i32";
  case TYPE_BOOL:
    return "bool";
  default:
    return "unknown";
  }
}

void ast_print(struct ast_node *node, int indent) {
  if (node == NULL)
    return;

  for (int i = 0; i < indent; i++)
    printf("  ");

  switch (node->type) {
  case AST_PROGRAM:
    printf("program\n");
    for (int i = 0; i < node->as.program.num_statements; i++) {
      ast_print(node->as.program.statements[i], indent + 1);
    }
    break;

  case AST_VARIABLE_DECL:
    printf("variable_decl: %.*s (type: %s, constant: %s)\n",
           (int)node->as.variable_decl.name.length,
           node->as.variable_decl.name.start,
           ast_data_type_to_string(node->as.variable_decl.type),
           node->as.variable_decl.is_constant ? "true" : "false");
    if (node->as.variable_decl.initialiser) {
      ast_print(node->as.variable_decl.initialiser, indent + 1);
    }
    break;

  case AST_GROUPING_EXPR:
    printf("grouping_expr:\n");
    ast_print(node->as.grouping_expr.expr, indent + 1);
    break;

  case AST_PRINT_STMT:
    printf("print_stmt:\n");
    ast_print(node->as.print_stmt.expr, indent + 1);
    break;

  case AST_BINARY_EXPR:
    printf("binary_expr: %.*s\n", (int)node->as.binary_expr.token.length,
           node->as.binary_expr.token.start);
    ast_print(node->as.binary_expr.left, indent + 1);
    ast_print(node->as.binary_expr.right, indent + 1);
    break;

  case AST_UNARY_EXPR:
    printf("unary_expr: %.*s\n", (int)node->as.unary_expr.token.length,
           node->as.unary_expr.token.start);
    ast_print(node->as.unary_expr.right, indent + 1);
    break;

  case AST_LITERAL_EXPR: {
    enum ast_data_type type = node->as.literal_expr.type;

    if (type == TYPE_I32) {
      printf("literal_expr: %d\n", node->as.literal_expr.as.i32);
    } else if (type == TYPE_BOOL) {
      printf("literal_expr: %s\n",
             node->as.literal_expr.as.boolean ? "true" : "false");
    }

    break;
  }

  case AST_IDENTIFIER_EXPR:
    printf("identifier_expr: %.*s\n", (int)node->as.idenitifer.length,
           node->as.idenitifer.start);
    break;

  default:
    UNREACHABLE();
  }
}
