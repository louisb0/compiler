#ifndef ast_h
#define ast_h

#include "lexer.h"

typedef struct ast_node ast_node;

typedef enum {
  AST_NODE_NUMBER,
  AST_NODE_UNARY,
  AST_NODE_BINARY,
} ast_node_type;

typedef struct {
  int value;
} ast_number;

typedef struct {
  char op;
  ast_node *right;
} ast_unary;

typedef struct {
  char op;
  ast_node *left;
  ast_node *right;
} ast_binary;

struct ast_node {
  ast_node_type type;
  lexer_token token;

  union {
    ast_number number;
    ast_unary unary;
    ast_binary binary;
  } as;
};

ast_node *ast_new(ast_node_type type, lexer_token token);
void ast_free(ast_node *root);
void ast_print(ast_node *root);

ast_node *ast_new_number(lexer_token token, int value);
ast_node *ast_new_binary(lexer_token token, ast_node *left, char op,
                         ast_node *right);
ast_node *ast_new_unary(lexer_token token, char op, ast_node *right);

#endif
