#include <stdio.h>
#include <string.h>

#include "compiler/ast.h"
#include "compiler/common.h"
#include "compiler/parser.h"
#include "compiler/resolver.h"
#include "compiler/scanner.h"
#include "compiler/symbols.h"
#include "compiler/typechecker.h"

char *read_file(const char *path) {
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
  if (argc < 2) {
    printf("[error] Usage: %s <file> [--dump-ast]\n", argv[0]);
    return 1;
  }

  bool dump_ast = (argc == 3 && strcmp(argv[2], "--dump-ast") == 0);
  char *src = read_file(argv[1]);

  scanner_t *scanner = scanner_new(src);
  parser_t *parser = parser_new(scanner);

  struct ast_node *root = NULL;
  if (!parser_run(parser, &root)) {
    goto cleanup;
  }

  symbol_table_t *table = symbol_table_new(100);
  if (!resolver_generate_table(root, table)) {
    goto cleanup;
  }

  if (typecheck(root, table) == TYPE_ERROR) {
    goto cleanup;
  }

  if (dump_ast) {
    ast_write_mermaid(root, "ast.svg");
  }

cleanup:
  free(src);
  ast_free(&root);
  parser_free(&parser);
  scanner_free(&scanner);
  symbol_table_free(&table);

  return root ? 0 : 1;
}
