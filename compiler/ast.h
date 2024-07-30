#ifndef ast_h
#define ast_h

#include "lexer.h"

typedef struct AST_Node AST_Node;

typedef enum {
  AST_NODE_NUMBER,
  AST_NODE_UNARY,
  AST_NODE_BINARY,
} AST_NodeType;

typedef struct {
  int value;
} AST_Number;

typedef struct {
  char op;
  AST_Node *right;
} AST_Unary;

typedef struct {
  char op;
  AST_Node *left;
  AST_Node *right;
} AST_Binary;

struct AST_Node {
  AST_NodeType type;
  Token token;

  union {
    AST_Number number;
    AST_Unary unary;
    AST_Binary binary;
  } as;
};

AST_Node *ast_new(AST_NodeType type, Token token);
void ast_free(AST_Node *root);
void ast_print(AST_Node *root);

AST_Node *ast_tmp_new_number(int value);
AST_Node *ast_tmp_new_binary(AST_Node *left, char op, AST_Node *right);
AST_Node *ast_tmp_new_unary(char op, AST_Node *right);

#endif
