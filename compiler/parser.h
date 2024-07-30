#ifndef parser_h
#define parser_h

#include "ast.h"
#include "lexer.h"

#define IS_BINARY_TOKEN(type)                                                  \
  ((type) == TOKEN_PLUS || (type) == TOKEN_MINUS || (type) == TOKEN_STAR ||    \
   (type) == TOKEN_SLASH)

#define IS_UNARY_TOKEN(type) ((type) == TOKEN_MINUS)

typedef struct parser_t parser_t;

parser_t *parser_new(lexer_t *lexer);
void parser_free(parser_t **parser);

ast_node *parser_run(parser_t *parser);

#endif
