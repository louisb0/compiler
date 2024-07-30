#include "ast.h"
#include "common.h"
#include "lexer.h"

Token tmp;

AST_Node *ast_new(AST_NodeType type, Token token) {
  AST_Node *node = (AST_Node *)malloc(sizeof(AST_Node));
  if (node == NULL) {
    ERROR_OUT();
  }

  node->type = type;
  node->token = token;

  return node;
}

void ast_free(AST_Node *root) {
  assert(root);

  switch (root->type) {
  case AST_NODE_NUMBER:
    free(root);
    break;
  case AST_NODE_BINARY: {
    ast_free(root->as.binary.left);
    ast_free(root->as.binary.right);
    free(root);
    break;
  }
  case AST_NODE_UNARY: {
    ast_free(root->as.unary.right);
    free(root);
    break;
  }
  }
}

void ast_print(AST_Node *root) {
  assert(root);

  switch (root->type) {
  case AST_NODE_NUMBER:
    printf("%d", root->as.number.value);
    break;
  case AST_NODE_BINARY: {
    printf("(");
    ast_print(root->as.binary.left);
    printf(" %c ", root->as.binary.op);
    ast_print(root->as.binary.right);
    printf(")");
    break;
  }
  case AST_NODE_UNARY: {
    printf("%c", root->as.unary.op);
    printf("(");
    ast_print(root->as.unary.right);
    printf(")");
    break;
  }
  }
}

AST_Node *ast_tmp_new_number(int value) {
  AST_Node *node = ast_new(AST_NODE_NUMBER, tmp);

  node->as.number.value = value;

  return node;
}

AST_Node *ast_tmp_new_binary(AST_Node *left, char op, AST_Node *right) {
  AST_Node *node = ast_new(AST_NODE_BINARY, tmp);

  node->as.binary.left = left;
  node->as.binary.op = op;
  node->as.binary.right = right;

  return node;
}

AST_Node *ast_tmp_new_unary(char op, AST_Node *right) {
  AST_Node *node = ast_new(AST_NODE_UNARY, tmp);

  node->as.unary.op = op;
  node->as.unary.right = right;

  return node;
}
