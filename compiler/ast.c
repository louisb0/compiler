#include "ast.h"
#include "common.h"
#include "lexer.h"

Token tmp;

ast_node *ast_new(ast_node_type type, Token token) {
  ast_node *node = (ast_node *)malloc(sizeof(ast_node));
  if (node == NULL) {
    ERROR_OUT();
  }

  node->type = type;
  node->token = token;

  return node;
}

void ast_free(ast_node *root) {
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

void ast_print(ast_node *root) {
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

ast_node *ast_tmp_new_number(int value) {
  ast_node *node = ast_new(AST_NODE_NUMBER, tmp);

  node->as.number.value = value;

  return node;
}

ast_node *ast_tmp_new_binary(ast_node *left, char op, ast_node *right) {
  ast_node *node = ast_new(AST_NODE_BINARY, tmp);

  node->as.binary.left = left;
  node->as.binary.op = op;
  node->as.binary.right = right;

  return node;
}

ast_node *ast_tmp_new_unary(char op, ast_node *right) {
  ast_node *node = ast_new(AST_NODE_UNARY, tmp);

  node->as.unary.op = op;
  node->as.unary.right = right;

  return node;
}
