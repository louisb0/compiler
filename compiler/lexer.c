#include <string.h>

#include "common.h"
#include "lexer.h"

typedef struct {
  const char *start;
  const char *current;
  int line;
} Lexer;

Lexer lexer;

void init_lexer(const char *source) {
  assert(source);

  lexer.start = source;
  lexer.current = source;
  lexer.line = 1;
}

static Token error_token(const char *message) {
  assert(message);

  Token token;
  token.type = TOKEN_ERROR;
  token.line = lexer.line;
  token.start = message;
  token.length = strlen(message);

  return token;
}

static Token make_token(TokenType type) {
  Token token;
  token.type = type;
  token.line = lexer.line;
  token.start = lexer.start;
  token.length = (int)(lexer.current - lexer.start);

  return token;
}

static bool is_at_end() { return *lexer.current == '\0'; }

static bool is_digit(char c) { return '0' <= c && c <= '9'; }

static char peek() { return *lexer.current; }

static char previous() { return *(lexer.current - 1); }

static char advance() {
  assert(!is_at_end());

  return *(lexer.current++);
}

static void skip_whitespace() {
  for (;;) {
    char c = peek();

    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      advance();
      break;

    case '\n':
      lexer.line++;
      advance();
      break;
    default:
      return;
    }
  }
}

static Token number() {
  assert(is_digit(previous()));

  while (is_digit(peek())) {
    advance();
  }

  return make_token(TOKEN_NUMBER);
}

Token lex_token() {
  skip_whitespace();

  lexer.start = lexer.current;

  if (is_at_end()) {
    return make_token(TOKEN_EOF);
  }

  char c = advance();

  if (is_digit(c)) {
    return number();
  }

  switch (c) {
  case '+':
    return make_token(TOKEN_PLUS);
  case '-':
    return make_token(TOKEN_MINUS);
  case '*':
    return make_token(TOKEN_STAR);
  case '/':
    return make_token(TOKEN_SLASH);
  }

  return error_token("Unknown character.");
}
