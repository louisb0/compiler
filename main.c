#include <stdio.h>

#include "compiler/lexer.h"

int main() {
  char *str = "1 + 2 / 3";
  init_lexer((const char *)str);

  for (;;) {
    Token t = lex_token();

    printf("%d: %.*s\n", t.type, t.length, t.start);

    if (t.type == TOKEN_EOF)
      break;
  }
}
