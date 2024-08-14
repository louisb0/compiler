#include "compiler/scanner.h"

int main() {
  const char *src = "var a: i32 = 1; const b: i32 = 2; print(a + b);";

  scanner_t *scanner = scanner_new(src);
  struct scanner_token token;
  while ((token = scanner_read_token(scanner)).type != TOKEN_EOF) {
    scanner_print_token(token);
  }
  scanner_print_token(token);

  return 0;
}
