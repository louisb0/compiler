#ifndef lexer_h
#define lexer_h

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

void init_lexer(const char *source);

Token lex_token();

#endif
