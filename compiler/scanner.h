#ifndef scanner_h
#define scanner_h

typedef struct scanner_t scanner_t;

enum scanner_token_type {
  // Keywords
  TOKEN_VAR,
  TOKEN_CONST,
  TOKEN_TRUE,
  TOKEN_FALSE,
  TOKEN_PRINT,

  // Types
  TOKEN_TYPE_I32,

  // Identifiers and literals
  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,

  // Operators
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,

  // Punctuation
  TOKEN_EQUAL,
  TOKEN_COLON,
  TOKEN_SEMICOLON,
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,

  // Special tokens
  TOKEN_EOF,
  TOKEN_ERROR
};

struct scanner_token {
  enum scanner_token_type type;

  const char *start;
  int length;
  int line;
};

scanner_t *scanner_new(const char *source);
void scanner_free(scanner_t **scanner);

struct scanner_token scanner_read_token(scanner_t *scanner);
void scanner_print_token(const struct scanner_token token);

#endif
