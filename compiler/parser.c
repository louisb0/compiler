#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.h"
#include "scanner.h"

// struct parser_error {
//   struct scanner_token token;
//   const char *message;
// };

struct parser_t {
  scanner_t *scanner;
  struct scanner_token current;
  struct scanner_token previous;

  bool panic_mode;
  bool had_error;
};

enum parser_precedence {
  PREC_NONE,
  PREC_ASSIGNMENT,
  PREC_TERM,
  PREC_FACTOR,
  PREC_UNARY,
  PREC_PRIMARY
};

typedef struct ast_node *(*parser_prefix_fn)(parser_t *parser);
typedef struct ast_node *(*parser_infix_fn)(parser_t *parser,
                                            struct ast_node *left);

struct parser_rule {
  parser_prefix_fn prefix;
  parser_infix_fn infix;

  enum parser_precedence prec;
};

parser_t *parser_new(scanner_t *scanner) {
  assert(scanner);

  parser_t *parser = (parser_t *)malloc(sizeof(*parser));
  if (parser == NULL) {
    ERROR_OUT();
  }

  parser->scanner = scanner;
  parser->panic_mode = false;
  parser->had_error = false;

  return parser;
}

void parser_free(parser_t **parser) {
  assert(parser && *parser);

  free(*parser);
  *parser = NULL;
}

static void error(parser_t *parser, struct scanner_token *token,
                  const char *message) {
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
    parser->current = scanner_read_token(parser->scanner);

    if (parser->current.type != TOKEN_ERROR)
      break;

    error(parser, &parser->current, parser->current.start);
  }
}

static void consume(parser_t *parser, enum scanner_token_type type,
                    const char *message) {
  if (parser->current.type == type) {
    advance(parser);
    return;
  }

  error(parser, &parser->current, message);
}

static bool check(parser_t *parser, enum scanner_token_type type) {
  return parser->current.type == type;
}

static bool match(parser_t *parser, enum scanner_token_type type) {
  if (!check(parser, type))
    return false;

  advance(parser);
  return true;
}

static void synchronise(parser_t *parser) {
  parser->panic_mode = false;

  while (parser->current.type != TOKEN_EOF) {
    if (parser->current.type == TOKEN_SEMICOLON)
      return;

    switch (parser->current.type) {
    case TOKEN_VAR:
    case TOKEN_CONST:
    case TOKEN_PRINT:
      return;

    default:;
    }

    advance(parser);
  }
}

static struct ast_node *parse_variable_decl(parser_t *parser);
static struct ast_node *parse_print_stmt(parser_t *parser);
static struct ast_node *parse_grouping_expr(parser_t *parser);
static struct ast_node *parse_binary_expr(parser_t *parser,
                                          struct ast_node *left);
static struct ast_node *parse_unary_expr(parser_t *parser);
static struct ast_node *parse_identifier_expr(parser_t *parser);
static struct ast_node *parse_literal_expr(parser_t *parser);

static struct ast_node *parse_precedence(parser_t *parser,
                                         enum parser_precedence prec);

struct parser_rule rules[] = {
    // Keywords
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_CONST] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {parse_literal_expr, NULL, PREC_NONE},
    [TOKEN_FALSE] = {parse_literal_expr, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},

    // Types
    [TOKEN_TYPE_I32] = {NULL, NULL, PREC_NONE},

    // Identifiers and literals
    [TOKEN_IDENTIFIER] = {parse_identifier_expr, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {parse_literal_expr, NULL, PREC_NONE},

    // Operators
    [TOKEN_PLUS] = {NULL, parse_binary_expr, PREC_TERM},
    [TOKEN_MINUS] = {parse_unary_expr, parse_binary_expr, PREC_TERM},
    [TOKEN_STAR] = {NULL, parse_binary_expr, PREC_FACTOR},
    [TOKEN_SLASH] = {NULL, parse_binary_expr, PREC_FACTOR},

    // Punctuation
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_COLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_PAREN] = {parse_grouping_expr, NULL, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},

    // Special tokens
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE}};

static struct parser_rule *get_rule(enum scanner_token_type type) {
  return &rules[type];
}

static struct ast_node *parse_decl(parser_t *parser);
static struct ast_node *parse_stmt(parser_t *parser);
static struct ast_node *parse_expr_stmt(parser_t *parser);
static struct ast_node *parse_expr(parser_t *parser);

static struct ast_node *parse_variable_decl(parser_t *parser) {
  bool is_constant = parser->previous.type == TOKEN_CONST;

  consume(parser, TOKEN_IDENTIFIER, "Expected variable name.");
  struct scanner_token name = parser->previous;

  consume(parser, TOKEN_COLON, "Expected ':' after variable name.");
  enum ast_data_type type;
  if (match(parser, TOKEN_TYPE_I32)) {
    type = TYPE_I32;
  } else if (match(parser, TOKEN_TYPE_BOOL)) {
    type = TYPE_BOOL;
  } else {
    error(parser, &parser->current, "Expected variable type.");
    return NULL;
  }

  consume(parser, TOKEN_EQUAL, "Expected equal after variable type.");
  struct ast_node *initializer = parse_expr(parser);

  consume(parser, TOKEN_SEMICOLON, "Expected ';' after variable declaration.");

  return ast_new_variable_decl(name, type, is_constant, initializer);
}

static struct ast_node *parse_print_stmt(parser_t *parser) {
  assert(parser->previous.type == TOKEN_PRINT);

  struct ast_node *expr = parse_expr(parser);
  consume(parser, TOKEN_SEMICOLON, "Expected ';' after value.");

  return ast_new_print_stmt(expr);
}

static struct ast_node *parse_grouping_expr(parser_t *parser) {
  assert(parser->previous.type == TOKEN_LEFT_PAREN);

  struct ast_node *expr = parse_expr(parser);
  consume(parser, TOKEN_RIGHT_PAREN, "Expected ')' after expression.");

  return ast_new_grouping_expr(expr);
}

static struct ast_node *parse_binary_expr(parser_t *parser,
                                          struct ast_node *left) {
  struct scanner_token op = parser->previous;

  assert(IS_BINARY_TOKEN(op.type));

  enum parser_precedence prec = get_rule(op.type)->prec + 1;
  return ast_new_binary_expr(op, left, parse_precedence(parser, prec));
}

static struct ast_node *parse_unary_expr(parser_t *parser) {
  struct scanner_token op = parser->previous;

  assert(IS_UNARY_TOKEN(op.type));

  enum parser_precedence prec = get_rule(op.type)->prec + 1;
  return ast_new_unary_expr(op, parse_precedence(parser, prec));
}

static struct ast_node *parse_identifier_expr(parser_t *parser) {
  assert(parser->previous.type == TOKEN_IDENTIFIER);

  return ast_new_identifier_expr(parser->previous);
}

static struct ast_node *parse_literal_expr(parser_t *parser) {
  assert(IS_LITERAL_TOKEN(parser->previous.type));

  switch (parser->previous.type) {
  case TOKEN_NUMBER: {
    char tmp[20];
    strncpy(tmp, parser->previous.start, parser->previous.length);
    tmp[parser->previous.length] = '\0';

    return ast_new_number_expr(atoi(tmp));
  }

  case TOKEN_TRUE:
  case TOKEN_FALSE:
    return ast_new_bool_expr(parser->previous.type == TOKEN_TRUE);

  default:
    UNREACHABLE();
    break;
  }
}

static struct ast_node *parse_decl(parser_t *parser) {
  struct ast_node *node = NULL;
  // TODO: potential side effect & bug?
  if (match(parser, TOKEN_VAR) || match(parser, TOKEN_CONST)) {
    node = parse_variable_decl(parser);
  } else {
    node = parse_stmt(parser);
  }

  if (parser->panic_mode)
    synchronise(parser);

  return node;
}

static struct ast_node *parse_stmt(parser_t *parser) {
  if (match(parser, TOKEN_PRINT)) {
    return parse_print_stmt(parser);
  } else {
    return parse_expr_stmt(parser);
  }
}

static struct ast_node *parse_expr_stmt(parser_t *parser) {
  struct ast_node *expr = parse_expr(parser);
  consume(parser, TOKEN_SEMICOLON, "Expected ';' after expression.");
  return expr;
}

static struct ast_node *parse_expr(parser_t *parser) {
  return parse_precedence(parser, PREC_ASSIGNMENT);
}

static struct ast_node *parse_precedence(parser_t *parser,
                                         enum parser_precedence prec) {
  assert(parser);

  advance(parser);

  parser_prefix_fn prefix = get_rule(parser->previous.type)->prefix;
  if (prefix == NULL) {
    error(parser, &parser->previous, "Expected an expression.");
    return NULL;
  }

  struct ast_node *left = prefix(parser);
  while (prec <= get_rule(parser->current.type)->prec) {
    if (parser->current.type == TOKEN_EOF)
      break;

    advance(parser);
    parser_infix_fn infix = get_rule(parser->previous.type)->infix;
    left = infix(parser, left);
  }

  return left;
}

bool parser_run(parser_t *parser, struct ast_node **output) {
  assert(parser);
  assert(output);

  advance(parser);

  struct ast_node *statements[MAX_STATEMENTS];

  int i = 0;
  while (!match(parser, TOKEN_EOF)) {
    statements[i++] = parse_decl(parser);
  }

  *output = ast_new_program(statements, i);

  return !parser->had_error;
}
