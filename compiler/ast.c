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

  case AST_ASSIGNMENT_STMT:
    ast_free(&(*root)->as.assignment_stmt.identifier);
    ast_free(&(*root)->as.assignment_stmt.expr);
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

struct ast_node *ast_new_assignment_stmt(struct ast_node *identifier,
                                         struct ast_node *expr) {
  struct ast_node *node = ast_new(AST_ASSIGNMENT_STMT);
  node->as.assignment_stmt.identifier = identifier;
  node->as.assignment_stmt.expr = expr;
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
  node->as.identifier = name;
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

// TODO: duplication with resolver
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
    printf("identifier_expr: %.*s\n", (int)node->as.identifier.length,
           node->as.identifier.start);
    break;

  default:
    UNREACHABLE();
  }
}

static void ast_print_mermaid(struct ast_node *node, int *node_count,
                              FILE *file) {
  if (node == NULL)
    return;

  int current_node = (*node_count)++;

  switch (node->type) {
  case AST_PROGRAM:
    fprintf(file, "  node%d[\"Program\"]\n", current_node);
    for (int i = 0; i < node->as.program.num_statements; i++) {
      int child_node = *node_count;
      ast_print_mermaid(node->as.program.statements[i], node_count, file);
      fprintf(file, "  node%d --> node%d\n", current_node, child_node);
    }
    break;

  case AST_VARIABLE_DECL:
    fprintf(file,
            "  node%d[\"Variable Decl: %.*s<br/>Type: %s<br/>Constant: %s\"]\n",
            current_node, (int)node->as.variable_decl.name.length,
            node->as.variable_decl.name.start,
            ast_data_type_to_string(node->as.variable_decl.type),
            node->as.variable_decl.is_constant ? "true" : "false");
    if (node->as.variable_decl.initialiser) {
      int child_node = *node_count;
      ast_print_mermaid(node->as.variable_decl.initialiser, node_count, file);
      fprintf(file, "  node%d --> node%d\n", current_node, child_node);
    }
    break;

  case AST_GROUPING_EXPR:
    fprintf(file, "  node%d[\"Grouping Expr\"]\n", current_node);
    int child_node = *node_count;
    ast_print_mermaid(node->as.grouping_expr.expr, node_count, file);
    fprintf(file, "  node%d --> node%d\n", current_node, child_node);
    break;

  case AST_PRINT_STMT:
    fprintf(file, "  node%d[\"Print Stmt\"]\n", current_node);
    child_node = *node_count;
    ast_print_mermaid(node->as.print_stmt.expr, node_count, file);
    fprintf(file, "  node%d --> node%d\n", current_node, child_node);
    break;

  case AST_ASSIGNMENT_STMT:
    fprintf(file, "  node%d[\"Assignment Stmt\"]\n", current_node);
    int identifier_node = *node_count;
    ast_print_mermaid(node->as.assignment_stmt.identifier, node_count, file);
    int expr_node = *node_count;
    ast_print_mermaid(node->as.assignment_stmt.expr, node_count, file);
    fprintf(file, "  node%d --> node%d\n", current_node, identifier_node);
    fprintf(file, "  node%d --> node%d\n", current_node, expr_node);
    break;

  case AST_BINARY_EXPR:
    fprintf(file, "  node%d[\"Binary Expr: %.*s\"]\n", current_node,
            (int)node->as.binary_expr.token.length,
            node->as.binary_expr.token.start);
    int left_node = *node_count;
    ast_print_mermaid(node->as.binary_expr.left, node_count, file);
    int right_node = *node_count;
    ast_print_mermaid(node->as.binary_expr.right, node_count, file);
    fprintf(file, "  node%d --> node%d\n", current_node, left_node);
    fprintf(file, "  node%d --> node%d\n", current_node, right_node);
    break;

  case AST_UNARY_EXPR:
    fprintf(file, "  node%d[\"Unary Expr: %.*s\"]\n", current_node,
            (int)node->as.unary_expr.token.length,
            node->as.unary_expr.token.start);
    child_node = *node_count;
    ast_print_mermaid(node->as.unary_expr.right, node_count, file);
    fprintf(file, "  node%d --> node%d\n", current_node, child_node);
    break;

  case AST_LITERAL_EXPR: {
    enum ast_data_type type = node->as.literal_expr.type;
    if (type == TYPE_I32) {
      fprintf(file, "  node%d[\"Literal: %d\"]\n", current_node,
              node->as.literal_expr.as.i32);
    } else if (type == TYPE_BOOL) {
      fprintf(file, "  node%d[\"Literal: %s\"]\n", current_node,
              node->as.literal_expr.as.boolean ? "true" : "false");
    }
    break;
  }

  case AST_IDENTIFIER_EXPR:
    fprintf(file, "  node%d[\"Identifier: %.*s\"]\n", current_node,
            (int)node->as.identifier.length, node->as.identifier.start);
    break;

  default:
    UNREACHABLE();
  }
}

int ast_write_mermaid(struct ast_node *root, const char *path) {
  FILE *file = fopen(path, "w");
  if (file == NULL)
    ERROR_OUT();

  fprintf(file, "graph TD\n");
  int node_count = 0;
  ast_print_mermaid(root, &node_count, file);

  fclose(file);

  char command[256];
  snprintf(command, sizeof(command), "npx mmdc -i %s -o %s", path, path);

  return system(command);
}
