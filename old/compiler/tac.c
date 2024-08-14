#include "tac.h"
#include "common.h"

#define VARIABLE_OPERAND(instruction_number)                                   \
  (tac_operand) {                                                              \
    .type = TAC_VARIABLE, .variable_number = (instruction_number)              \
  }

#define NUMBER_OPERAND(imm)                                                    \
  (tac_operand) { .type = TAC_NUMBER, .immediate = (imm) }

#define NO_OPERAND()                                                           \
  (tac_operand) { .type = TAC_NONE }

static tac_instruction *tac_generate_program(tac_program *program,
                                             ast_node *root);
static tac_instruction *tac_create_instruction(tac_program *program);
static void tac_print_instruction(const tac_instruction *instruction);

tac_program *tac_new(ast_node *root) {
  assert(root);

  tac_program *program = malloc(sizeof(*program));
  if (!program) {
    ERROR_OUT();
  }

  program->head = NULL;
  program->tail = NULL;
  program->instruction_count = 0;

  tac_generate_program(program, root);
  return program;
}

static tac_instruction *tac_generate_program(tac_program *program,
                                             ast_node *root) {
  assert(root && program);

  tac_instruction *new;
  switch (root->type) {
  case AST_NUMBER_EXPR: {
    new = tac_create_instruction(program);

    new->op = TAC_OP_ASSIGN;
    new->result = VARIABLE_OPERAND(program->instruction_count);
    new->arg1 = NUMBER_OPERAND(root->as.number.value);
    new->arg2 = NO_OPERAND();
    break;
  }
  case AST_BINARY_EXPR: {
    tac_instruction *left = tac_generate_program(program, root->as.binary.left);
    tac_instruction *right =
        tac_generate_program(program, root->as.binary.right);

    new = tac_create_instruction(program);

    switch (root->token.type) {
    case TOKEN_PLUS:
      new->op = TAC_OP_ADD;
      break;
    case TOKEN_MINUS:
      new->op = TAC_OP_SUB;
      break;
    case TOKEN_STAR:
      new->op = TAC_OP_MUL;
      break;
    case TOKEN_SLASH:
      new->op = TAC_OP_DIV;
      break;
    default:
      UNREACHABLE();
    }

    new->result = VARIABLE_OPERAND(program->instruction_count);
    new->arg1 = VARIABLE_OPERAND(left->result.variable_number);
    new->arg2 = VARIABLE_OPERAND(right->result.variable_number);
    break;
  }
  case AST_UNARY_EXPR: {
    tac_instruction *right =
        tac_generate_program(program, root->as.unary.right);

    new = tac_create_instruction(program);

    new->op = TAC_OP_NEG;
    new->result = VARIABLE_OPERAND(program->instruction_count);
    new->arg1 = VARIABLE_OPERAND(right->result.variable_number);
    break;
  }
  }

  return new;
}

static tac_instruction *tac_create_instruction(tac_program *program) {
  tac_instruction *new = malloc(sizeof(*new));
  if (new == NULL) {
    ERROR_OUT();
    return NULL;
  }

  new->next = NULL;

  if (program->tail) {
    program->tail->next = new;
  } else {
    program->head = new;
  }
  program->tail = new;
  program->instruction_count++;

  return new;
}

void tac_free(tac_program *program) {
  assert(program);

  tac_instruction *current = program->head;
  while (current) {
    tac_instruction *next = current->next;
    free(current);
    current = next;
  }

  free(program);
}

void tac_print(const tac_program *program) {
  if (!program)
    return;

  for (tac_instruction *instr = program->head; instr; instr = instr->next) {
    tac_print_instruction(instr);
    printf("\n");
  }
}

static void tac_print_instruction(const tac_instruction *instruction) {
  if (!instruction)
    return;

  switch (instruction->op) {
  case TAC_OP_ASSIGN:
    printf("t%d <- %d", instruction->result.variable_number,
           instruction->arg1.immediate);
    break;
  case TAC_OP_ADD:
    printf("t%d <- add t%d, t%d", instruction->result.variable_number,
           instruction->arg1.variable_number,
           instruction->arg2.variable_number);
    break;
  case TAC_OP_SUB:
    printf("t%d <- sub t%d, t%d", instruction->result.variable_number,
           instruction->arg1.variable_number,
           instruction->arg2.variable_number);
    break;
  case TAC_OP_MUL:
    printf("t%d <- mul t%d, t%d", instruction->result.variable_number,
           instruction->arg1.variable_number,
           instruction->arg2.variable_number);
    break;
  case TAC_OP_DIV:
    printf("t%d <- div t%d, t%d", instruction->result.variable_number,
           instruction->arg1.variable_number,
           instruction->arg2.variable_number);
    break;
  case TAC_OP_NEG:
    printf("t%d <- neg t%d", instruction->result.variable_number,
           instruction->arg1.variable_number);
    break;
  }
}
