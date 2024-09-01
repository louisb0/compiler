#include <stdio.h>

#include "compiler/ast.h"
#include "compiler/common.h"
#include "compiler/parser.h"
#include "compiler/resolver.h"
#include "compiler/scanner.h"
#include "compiler/symbols.h"
#include "compiler/typechecker.h"

const char *read_file(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL)
    ERROR_OUT();

  fseek(file, 0L, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  char *buffer = (char *)malloc(size + 1);
  if (buffer == NULL)
    ERROR_OUT();

  size_t bytes_read = fread(buffer, sizeof(char), size, file);
  if (bytes_read < size)
    ERROR_OUT();

  fclose(file);

  buffer[bytes_read] = '\0';
  return buffer;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("[error] Usage: %s <file>\n", argv[0]);
    return 1;
  }

  const char *src = read_file(argv[1]);

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
