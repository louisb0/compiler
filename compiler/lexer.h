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
} lexer_token_type;

typedef struct {
  lexer_token_type type;

  const char *start;
  int length;
  int line;
} lexer_token;

lexer_t *lexer_new(const char *source);
void lexer_free(lexer_t **lexer);

lexer_token lexer_read_token(lexer_t *lexer);

#endif
