#ifndef parser_h
#define parser_h

#include "ast.h"
#include "common.h"
#include "scanner.h"

#define IS_BINARY_TOKEN(type)                                                  \
  ((type) == TOKEN_PLUS || (type) == TOKEN_MINUS || (type) == TOKEN_STAR ||    \
   (type) == TOKEN_SLASH)

#define IS_UNARY_TOKEN(type) ((type) == TOKEN_MINUS)

#define IS_LITERAL_TOKEN(type)                                                 \
  ((type) == TOKEN_TRUE || (type) == TOKEN_FALSE || (type) == TOKEN_NUMBER)

typedef struct parser_t parser_t;

parser_t *parser_new(scanner_t *scanner);
void parser_free(parser_t **parser);

bool parser_run(parser_t *parser, struct ast_node **output);

#endif
