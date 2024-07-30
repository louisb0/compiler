#ifndef ast_h
#define ast_h

#include "lexer.h"

typedef struct ast_node ast_node;

typedef enum {
  AST_NUMBER_EXPR,
  AST_UNARY_EXPR,
  AST_BINARY_EXPR,
} ast_node_type;

typedef struct {
  int value;
} ast_number_expr;

typedef struct {
  ast_node *right;
} ast_unary_expr;

typedef struct {
  ast_node *left;
  ast_node *right;
} ast_binary_expr;

struct ast_node {
  ast_node_type type;
  lexer_token token;

  union {
    ast_number_expr number;
    ast_unary_expr unary;
    ast_binary_expr binary;
  } as;
};

ast_node *ast_new(ast_node_type type, lexer_token token);
void ast_free(ast_node **root);
void ast_print(ast_node *root);

ast_node *ast_new_number(lexer_token token, int value);
ast_node *ast_new_binary(lexer_token token, ast_node *left, ast_node *right);
ast_node *ast_new_unary(lexer_token token, ast_node *right);

#endif
