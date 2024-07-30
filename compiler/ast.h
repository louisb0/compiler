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
} AST_Number;

typedef struct {
  char op;
  ast_node *right;
} AST_Unary;

typedef struct {
  char op;
  ast_node *left;
  ast_node *right;
} AST_Binary;

struct ast_node {
  ast_node_type type;
  Token token;

  union {
    AST_Number number;
    AST_Unary unary;
    AST_Binary binary;
  } as;
};

ast_node *ast_new(ast_node_type type, Token token);
void ast_free(ast_node *root);
void ast_print(ast_node *root);

ast_node *ast_tmp_new_number(int value);
ast_node *ast_tmp_new_binary(ast_node *left, char op, ast_node *right);
ast_node *ast_tmp_new_unary(char op, ast_node *right);

#endif
