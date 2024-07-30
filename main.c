#include "compiler/ast.h"

int main() {
  AST_Node *one = ast_tmp_new_number(1);
  AST_Node *two = ast_tmp_new_number(2);
  AST_Node *three = ast_tmp_new_number(3);

  AST_Node *sum = ast_tmp_new_binary(one, '+', two);
  AST_Node *divide = ast_tmp_new_binary(sum, '/', three);

  ast_print(divide);
  ast_free(divide);
}
