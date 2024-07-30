#include "ast.h"
#include "common.h"
#include "lexer.h"

ast_node *ast_new(ast_node_type type, lexer_token token) {
  ast_node *node = (ast_node *)malloc(sizeof(*node));
  if (node == NULL) {
    ERROR_OUT();
  }

  node->type = type;
  node->token = token;

  return node;
}

void ast_free(ast_node **root) {
  assert(root && *root);

  switch ((*root)->type) {
  case AST_NUMBER_EXPR:
    free((*root));
    *root = NULL;
    break;
  case AST_BINARY_EXPR: {
    ast_free(&(*root)->as.binary.left);
    ast_free(&(*root)->as.binary.right);

    free(*root);
    *root = NULL;
    break;
  }
  case AST_UNARY_EXPR: {
    ast_free(&(*root)->as.unary.right);

    free((*root));
    *root = NULL;
    break;
  }
  }
}

void ast_print(ast_node *root) {
  assert(root);

  switch (root->type) {
  case AST_NUMBER_EXPR:
    printf("%d", root->as.number.value);
    break;
  case AST_BINARY_EXPR: {
    printf("(");
    ast_print(root->as.binary.left);
    printf(" %c ", root->token.start[0]);
    ast_print(root->as.binary.right);
    printf(")");
    break;
  }
  case AST_UNARY_EXPR: {
    printf("%c", root->token.start[0]);
    printf("(");
    ast_print(root->as.unary.right);
    printf(")");
    break;
  }
  }
}

ast_node *ast_new_number(lexer_token token, int value) {
  ast_node *node = ast_new(AST_NUMBER_EXPR, token);

  node->as.number.value = value;

  return node;
}

ast_node *ast_new_binary(lexer_token token, ast_node *left, ast_node *right) {
  ast_node *node = ast_new(AST_BINARY_EXPR, token);

  node->as.binary.left = left;
  node->as.binary.right = right;

  return node;
}

ast_node *ast_new_unary(lexer_token token, ast_node *right) {
  ast_node *node = ast_new(AST_UNARY_EXPR, token);

  node->as.unary.right = right;

  return node;
}
