#ifndef ast_h
#define ast_h

#include "common.h"
#include "scanner.h"

#define MAX_STATEMENTS 100

enum ast_node_type {
  AST_PROGRAM,
  AST_VARIABLE_DECL,
  AST_PRINT_STMT,
  AST_ASSIGNMENT_STMT,
  AST_GROUPING_EXPR,
  AST_BINARY_EXPR,
  AST_LITERAL_EXPR,
  AST_IDENTIFIER_EXPR,
  AST_UNARY_EXPR,
};

enum ast_data_type { TYPE_I32, TYPE_BOOL, TYPE_ERROR };

struct ast_program {
  struct ast_node *statements[MAX_STATEMENTS];
  int num_statements;
};

struct ast_variable_decl {
  struct scanner_token name;
  enum ast_data_type type;
  bool is_constant;

  struct ast_node *initialiser;
};

struct ast_expr_stmt {
  struct ast_node *expr;
};

struct ast_print_stmt {
  struct ast_node *expr;
};

struct ast_assignment_stmt {
  struct ast_node *identifier;
  struct ast_node *expr;
};

struct ast_grouping_expr {
  struct ast_node *expr;
};

struct ast_binary_expr {
  struct scanner_token token;

  struct ast_node *left;
  struct ast_node *right;
};

struct ast_unary_expr {
  struct scanner_token token;

  struct ast_node *right;
};

struct ast_literal_expr {
  enum ast_data_type type;

  union {
    int i32;
    bool boolean;
  } as;
};

struct ast_node {
  enum ast_node_type type;

  union {
    struct ast_program program;
    struct ast_variable_decl variable_decl;
    struct ast_expr_stmt expr_stmt;
    struct ast_print_stmt print_stmt;
    struct ast_assignment_stmt assignment_stmt;
    struct ast_grouping_expr grouping_expr;
    struct ast_binary_expr binary_expr;
    struct ast_unary_expr unary_expr;
    struct ast_literal_expr literal_expr;

    struct scanner_token identifier;
  } as;
};

struct ast_node *ast_new(enum ast_node_type type);
void ast_free(struct ast_node **root);

struct ast_node *ast_new_program(struct ast_node **statements,
                                 int num_statements);
struct ast_node *ast_new_variable_decl(struct scanner_token name,
                                       enum ast_data_type type,
                                       bool is_constant,
                                       struct ast_node *initialiser);
struct ast_node *ast_new_print_stmt(struct ast_node *expr);
struct ast_node *ast_new_assignment_stmt(struct ast_node *identifier,
                                         struct ast_node *expr);
struct ast_node *ast_new_grouping_expr(struct ast_node *expr);
struct ast_node *ast_new_binary_expr(struct scanner_token token,
                                     struct ast_node *left,
                                     struct ast_node *right);
struct ast_node *ast_new_unary_expr(struct scanner_token token,
                                    struct ast_node *right);
struct ast_node *ast_new_identifier_expr(struct scanner_token name);
struct ast_node *ast_new_number_expr(int literal);
struct ast_node *ast_new_bool_expr(bool literal);

void ast_print(struct ast_node *root, int ident);
int ast_write_mermaid(struct ast_node *root, const char *path);

#endif
