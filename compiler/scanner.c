#include <string.h>

#include "common.h"
#include "scanner.h"

struct scanner_t {
  const char *start;
  const char *current;
  int line;
};

scanner_t *scanner_new(const char *source) {
  assert(source);

  scanner_t *scanner = (scanner_t *)malloc(sizeof(*scanner));
  if (scanner == NULL) {
    ERROR_OUT();
  }

  scanner->start = source;
  scanner->current = source;
  scanner->line = 1;

  return scanner;
}

void scanner_free(scanner_t **scanner) {
  assert(scanner && *scanner);

  free(*scanner);
  *scanner = NULL;
}

static struct scanner_token error_token(const char *message, int line) {
  assert(message);

  struct scanner_token token;
  token.type = TOKEN_ERROR;
  token.line = line;
  token.start = message;
  token.length = strlen(message);

  return token;
}

static struct scanner_token make_token(scanner_t *scanner,
                                       enum scanner_token_type type) {
  assert(scanner);

  struct scanner_token token;
  token.type = type;
  token.line = scanner->line;
  token.start = scanner->start;
  token.length = (int)(scanner->current - scanner->start);

  return token;
}

static bool is_at_end(scanner_t *scanner) {
  assert(scanner);

  return *scanner->current == '\0';
}

static bool is_digit(char c) { return '0' <= c && c <= '9'; }

static bool is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static char peek_next(scanner_t *scanner) {
  assert(scanner);

  return *(scanner->current + 1);
}

static char peek(scanner_t *scanner) {
  assert(scanner);

  return *scanner->current;
}

static char previous(scanner_t *scanner) {
  assert(scanner);

  return *(scanner->current - 1);
}

static char advance(scanner_t *scanner) {
  assert(scanner);
  assert(!is_at_end(scanner));

  return *(scanner->current++);
}

static void skip_whitespace(scanner_t *scanner) {
  assert(scanner);

  for (;;) {
    char c = peek(scanner);

    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      advance(scanner);
      break;

    case '/':
      if (peek_next(scanner) == '/') {
        while (peek(scanner) != '\n' && !is_at_end(scanner))
          advance(scanner);
      } else {
        return;
      }
      break;

    case '\n':
      scanner->line++;
      advance(scanner);
      break;
    default:
      return;
    }
  }
}

static enum scanner_token_type check_keyword(scanner_t *scanner, int start,
                                             int length, const char *rest,
                                             enum scanner_token_type type) {
  if (scanner->current - scanner->start == start + length &&
      memcmp(scanner->start + start, rest, length) == 0) {
    return type;
  }

  return TOKEN_IDENTIFIER;
}

static enum scanner_token_type identifier_type(scanner_t *scanner) {
  switch (scanner->start[0]) {
  case 'b':
    return check_keyword(scanner, 1, 3, "ool", TOKEN_TYPE_BOOL);
  case 'c':
    return check_keyword(scanner, 1, 4, "onst", TOKEN_CONST);
  case 'f':
    return check_keyword(scanner, 1, 4, "alse", TOKEN_FALSE);
  case 'i':
    return check_keyword(scanner, 1, 2, "32", TOKEN_TYPE_I32);
  case 'p':
    return check_keyword(scanner, 1, 4, "rint", TOKEN_PRINT);
  case 't':
    return check_keyword(scanner, 1, 3, "rue", TOKEN_TRUE);
  case 'v':
    return check_keyword(scanner, 1, 2, "ar", TOKEN_VAR);
  }

  return TOKEN_IDENTIFIER;
}

static struct scanner_token number(scanner_t *scanner) {
  assert(scanner);
  assert(is_digit(previous(scanner)));

  while (is_digit(peek(scanner))) {
    advance(scanner);
  }

  return make_token(scanner, TOKEN_NUMBER);
}

static struct scanner_token identifier(scanner_t *scanner) {
  while (is_alpha(peek(scanner)) || is_digit(peek(scanner))) {
    advance(scanner);
  }

  return make_token(scanner, identifier_type(scanner));
}

struct scanner_token scanner_read_token(scanner_t *scanner) {
  assert(scanner);

  skip_whitespace(scanner);

  scanner->start = scanner->current;

  if (is_at_end(scanner)) {
    return make_token(scanner, TOKEN_EOF);
  }

  char c = advance(scanner);

  if (is_digit(c))
    return number(scanner);

  if (is_alpha(c))
    return identifier(scanner);

  switch (c) {
  case '+':
    return make_token(scanner, TOKEN_PLUS);
  case '-':
    return make_token(scanner, TOKEN_MINUS);
  case '*':
    return make_token(scanner, TOKEN_STAR);
  case '/':
    return make_token(scanner, TOKEN_SLASH);
  case '=':
    return make_token(scanner, TOKEN_EQUAL);
  case ':':
    return make_token(scanner, TOKEN_COLON);
  case ';':
    return make_token(scanner, TOKEN_SEMICOLON);
  case '(':
    return make_token(scanner, TOKEN_LEFT_PAREN);
  case ')':
    return make_token(scanner, TOKEN_RIGHT_PAREN);
  }

  return error_token("Unknown character.", scanner->line);
}

void scanner_print_token(const struct scanner_token token) {
  printf("Token {\n");
  printf("  Type: ");

  switch (token.type) {
  case TOKEN_VAR:
    printf("VAR");
    break;
  case TOKEN_CONST:
    printf("CONST");
    break;
  case TOKEN_TRUE:
    printf("TRUE");
    break;
  case TOKEN_FALSE:
    printf("FALSE");
    break;
  case TOKEN_PRINT:
    printf("PRINT");
    break;
  case TOKEN_TYPE_I32:
    printf("TYPE_I32");
    break;
  case TOKEN_IDENTIFIER:
    printf("IDENTIFIER");
    break;
  case TOKEN_NUMBER:
    printf("NUMBER");
    break;
  case TOKEN_PLUS:
    printf("PLUS");
    break;
  case TOKEN_MINUS:
    printf("MINUS");
    break;
  case TOKEN_STAR:
    printf("STAR");
    break;
  case TOKEN_SLASH:
    printf("SLASH");
    break;
  case TOKEN_EQUAL:
    printf("EQUAL");
    break;
  case TOKEN_COLON:
    printf("COLON");
    break;
  case TOKEN_SEMICOLON:
    printf("SEMICOLON");
    break;
  case TOKEN_LEFT_PAREN:
    printf("LEFT_PAREN");
    break;
  case TOKEN_RIGHT_PAREN:
    printf("RIGHT_PAREN");
    break;
  case TOKEN_EOF:
    printf("EOF");
    break;
  case TOKEN_ERROR:
    printf("ERROR");
    break;
  default:
    printf("UNKNOWN");
  }

  printf("\n  Lexeme: ");
  if (token.start != NULL) {
    printf("\"%.*s\"", token.length, token.start);
  } else {
    printf("NULL");
  }

  printf("\n  Length: %d", token.length);
  printf("\n  Line: %d", token.line);
  printf("\n}\n");
}
