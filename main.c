#include "compiler/ast.h"

int main() {
  lexer_token tmp;

  ast_node *one = ast_new_number(tmp, 1);
  ast_node *two = ast_new_number(tmp, 2);
  ast_node *three = ast_new_number(tmp, 3);

  ast_node *sum = ast_new_binary(tmp, one, '+', two);
  ast_node *divide = ast_new_binary(tmp, sum, '/', three);

  ast_print(divide);
  ast_free(divide);
}
