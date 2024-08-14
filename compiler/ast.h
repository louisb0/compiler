#ifndef ast_h
#define ast_h

#include "common.h"
#include "scanner.h"

#define MAX_STATEMENTS 100

enum ast_node_type {
  AST_PROGRAM,
  AST_VARIABLE_DECL,
  AST_PRINT_STMT,
  AST_BINARY_EXPR,
  AST_LITERAL,
  AST_VARIABLE,
  AST_UNARY,
};

enum ast_data_type {
  TYPE_I32,
};

struct ast_program {
  struct ast_node *statements[MAX_STATEMENTS];
  unsigned int num_statements;
};

struct ast_variable_decl {
  struct scanner_token name;
  enum ast_data_type type;
  bool is_constant;

  struct ast_node *initialiser;
};

struct ast_print_stmt {
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

struct ast_node {
  enum ast_node_type type;

  union {
    struct ast_program program;
    struct ast_variable_decl variable_decl;
    struct ast_print_stmt print_stmt;
    struct ast_binary_expr binary_expr;
    struct ast_unary_expr unary_expr;

    struct scanner_token idenitifer;
    int literal;
  } as;
};

#endif
