#include "compiler/ast.h"
#include "compiler/parser.h"
#include "compiler/resolver.h"
#include "compiler/scanner.h"
#include "compiler/symbols.h"
#include "compiler/typechecker.h"

int main() {
  const char *src = "var a: i32 = 1; const b: i32 = 2;"
                    // "print(a + b / 2 * (-3 - 1));"
                    "var c: bool = true;"
                    "print(a+c);";

  scanner_t *scanner = scanner_new(src);
  parser_t *parser = parser_new(scanner);

  struct ast_node *root = NULL;
  if (!parser_run(parser, &root)) {
    return 1;
  }

  symbol_table_t *table = symbol_table_new(100);
  if (!resolver_generate_table(root, table)) {
    return 1;
  }

  if (typecheck(root, table) == TYPE_ERROR) {
    return 1;
  }

  symbol_table_free(&table);
  ast_free(&root);
  parser_free(&parser);
  scanner_free(&scanner);

  return 0;
}
