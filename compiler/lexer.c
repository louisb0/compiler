#include <string.h>

#include "common.h"
#include "lexer.h"

struct lexer_t {
  const char *start;
  const char *current;
  int line;
};

lexer_t *lexer_new(const char *source) {
  assert(source);

  lexer_t *lexer = (lexer_t *)malloc(sizeof(*lexer));
  if (lexer == NULL) {
    ERROR_OUT();
  }

  lexer->start = source;
  lexer->current = source;
  lexer->line = 1;

  return lexer;
}

void lexer_free(lexer_t **lexer) {
  assert(lexer && *lexer);

  free(*lexer);
  *lexer = NULL;
}

static lexer_token error_token(const char *message, int line) {
  assert(message);

  lexer_token token;
  token.type = TOKEN_ERROR;
  token.line = line;
  token.start = message;
  token.length = strlen(message);

  return token;
}

static lexer_token make_token(lexer_t *lexer, lexer_token_type type) {
  assert(lexer);

  lexer_token token;
  token.type = type;
  token.line = lexer->line;
  token.start = lexer->start;
  token.length = (int)(lexer->current - lexer->start);

  return token;
}

static bool is_at_end(lexer_t *lexer) {
  assert(lexer);

  return *lexer->current == '\0';
}

static bool is_digit(char c) { return '0' <= c && c <= '9'; }

static char peek(lexer_t *lexer) {
  assert(lexer);

  return *lexer->current;
}

static char previous(lexer_t *lexer) {
  assert(lexer);

  return *(lexer->current - 1);
}

static char advance(lexer_t *lexer) {
  assert(lexer);
  assert(!is_at_end(lexer));

  return *(lexer->current++);
}

static void skip_whitespace(lexer_t *lexer) {
  assert(lexer);

  for (;;) {
    char c = peek(lexer);

    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      advance(lexer);
      break;

    case '\n':
      lexer->line++;
      advance(lexer);
      break;
    default:
      return;
    }
  }
}

static lexer_token number(lexer_t *lexer) {
  assert(lexer);
  assert(is_digit(previous(lexer)));

  while (is_digit(peek(lexer))) {
    advance(lexer);
  }

  return make_token(lexer, TOKEN_NUMBER);
}

lexer_token lexer_read_token(lexer_t *lexer) {
  assert(lexer);

  skip_whitespace(lexer);

  lexer->start = lexer->current;

  if (is_at_end(lexer)) {
    return make_token(lexer, TOKEN_EOF);
  }

  char c = advance(lexer);

  if (is_digit(c)) {
    return number(lexer);
  }

  switch (c) {
  case '+':
    return make_token(lexer, TOKEN_PLUS);
  case '-':
    return make_token(lexer, TOKEN_MINUS);
  case '*':
    return make_token(lexer, TOKEN_STAR);
  case '/':
    return make_token(lexer, TOKEN_SLASH);
  }

  return error_token("Unknown character.", lexer->line);
}
