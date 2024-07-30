#ifndef lexer_h
#define lexer_h

typedef struct lexer_t lexer_t;

typedef enum {
  TOKEN_NUMBER,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_EOF,
  TOKEN_ERROR,
} TokenType;

typedef struct {
  TokenType type;

  const char *start;
  int length;
  int line;
} Token;

lexer_t *lexer_new(const char *source);
void lexer_free(lexer_t **lexer);

Token lexer_read_token(lexer_t *lexer);

#endif
