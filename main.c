#include "compiler/ast.h"
#include "compiler/parser.h"
#include "compiler/resolver.h"
#include "compiler/scanner.h"
#include "compiler/symbols.h"

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

  symbol_table_t *table = symbol_table_new(100);
  resolver_generate_table(root, table);

  struct scanner_token a = {.start = "a", .length = 1};
  struct scanner_token b = {.start = "b", .length = 1};

  ast_print(symbol_table_get(table, &a), 0);
  ast_print(symbol_table_get(table, &b), 0);

  symbol_table_free(&table);
  ast_free(&root);
  parser_free(&parser);
  scanner_free(&scanner);

  return 0;
}
