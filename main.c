#include "compiler/ast.h"
#include "compiler/parser.h"
#include "compiler/scanner.h"

int main() {
  const char *src = "var a: i32 = 1; const b: i32 = 2;"
                    "print(a + b / 2 * (-3 - 1));";

  scanner_t *scanner = scanner_new(src);
  parser_t *parser = parser_new(scanner);

  struct ast_node *root = NULL;
  if (!parser_run(parser, &root)) {
    printf("Parse error. Exiting...\n");
    return 1;
  }

  ast_print(root, 0);

  return 0;
}
