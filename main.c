#include "compiler/ast.h"
#include "compiler/common.h"
#include "compiler/lexer.h"
#include "compiler/parser.h"

int main(int argc, char *argv[]) {
  lexer_t *lexer = lexer_new((const char *)argv[1]);
  parser_t *parser = parser_new(lexer);

  ast_node *head = parser_run(parser);
  if (head == NULL) {
    ERROR_OUT();
  }

  ast_print(head);

  ast_free(&head);
  lexer_free(&lexer);
  parser_free(&parser);
}
