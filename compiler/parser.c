#include <string.h>

#include "ast.h"
#include "common.h"
#include "lexer.h"
#include "parser.h"

struct parser_t {
  lexer_t *lexer;
  lexer_token current;
  lexer_token previous;

  bool panic_mode;
  bool had_error;
};

typedef enum {
  PREC_NONE,
  PREC_TERM,   // + -
  PREC_FACTOR, // * /
  PREC_UNARY,  // ! -
  PREC_PRIMARY
} parser_precedence;

typedef ast_node *(*parser_prefix_fn)(parser_t *parser);
typedef ast_node *(*parser_infix_fn)(parser_t *parser, ast_node *left);

typedef struct {
  parser_prefix_fn prefix;
  parser_infix_fn infix;
  parser_precedence prec;
} parser_rule;

parser_t *parser_new(lexer_t *lexer) {
  assert(lexer);

  parser_t *parser = (parser_t *)malloc(sizeof(*parser));
  if (parser == NULL) {
    ERROR_OUT();
  }

  parser->lexer = lexer;
  parser->panic_mode = false;
  parser->had_error = false;

  return parser;
}

void parser_free(parser_t **parser) {
  assert(parser && *parser);

  free(*parser);
  *parser = NULL;
}

static void error(parser_t *parser, lexer_token *token, const char *message) {
  assert(token != NULL);
  assert(message != NULL);

  if (parser->panic_mode)
    return;

  parser->panic_mode = true;
  fprintf(stderr, "line %d - error", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token->type == TOKEN_ERROR) {

  } else {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, " - %s\n", message);
  parser->had_error = true;
}

static void advance(parser_t *parser) {
  assert(parser);
  assert(parser->current.type != TOKEN_ERROR);
  assert(parser->previous.type != TOKEN_EOF);

  parser->previous = parser->current;

  for (;;) {
    parser->current = lexer_read_token(parser->lexer);

    if (parser->current.type != TOKEN_ERROR)
      break;

    error(parser, &parser->current, parser->current.start);
  }
}

static ast_node *parse_number(parser_t *parser);
static ast_node *parse_binary(parser_t *parser, ast_node *left);
static ast_node *parse_unary(parser_t *parser);

static ast_node *parse_precedence(parser_t *parser, parser_precedence prec);

parser_rule rules[] = {[TOKEN_NUMBER] = {parse_number, NULL, PREC_NONE},
                       [TOKEN_PLUS] = {NULL, parse_binary, PREC_TERM},
                       [TOKEN_MINUS] = {parse_unary, parse_binary, PREC_TERM},
                       [TOKEN_STAR] = {NULL, parse_binary, PREC_FACTOR},
                       [TOKEN_SLASH] = {NULL, parse_binary, PREC_FACTOR},
                       [TOKEN_ERROR] = {NULL, NULL, PREC_NONE}};

static parser_rule *get_rule(lexer_token_type type) { return &rules[type]; }

static ast_node *parse_number(parser_t *parser) {
  assert(parser->previous.type == TOKEN_NUMBER);

  char tmp[20];
  strncpy(tmp, parser->previous.start, parser->previous.length);
  tmp[parser->previous.length] = '\0';

  return ast_new_number(parser->previous, atoi(tmp));
}

static ast_node *parse_binary(parser_t *parser, ast_node *left) {
  lexer_token op = parser->previous;

  assert(IS_BINARY_TOKEN(op.type));

  parser_precedence prec = get_rule(op.type)->prec + 1;
  return ast_new_binary(op, left, parse_precedence(parser, prec));
}

static ast_node *parse_unary(parser_t *parser) {
  lexer_token op = parser->previous;

  assert(IS_UNARY_TOKEN(op.type));

  parser_precedence prec = get_rule(op.type)->prec + 1;
  return ast_new_unary(op, parse_precedence(parser, prec));
}

static ast_node *parse_precedence(parser_t *parser, parser_precedence prec) {
  assert(parser);

  advance(parser);

  parser_prefix_fn prefix = get_rule(parser->previous.type)->prefix;
  if (prefix == NULL) {
    error(parser, &parser->previous, "Expected an expression.");
    return NULL;
  }

  ast_node *left = prefix(parser);
  while (prec <= get_rule(parser->current.type)->prec) {
    if (parser->current.type == TOKEN_EOF)
      break;

    advance(parser);
    parser_infix_fn infix = get_rule(parser->previous.type)->infix;
    left = infix(parser, left);
  }

  return left;
}

ast_node *parser_run(parser_t *parser) {
  advance(parser);

  return parse_precedence(parser, PREC_NONE);
}
