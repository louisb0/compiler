#ifndef tac_h
#define tac_h

#include "ast.h"

typedef enum { TAC_NUMBER, TAC_VARIABLE, TAC_NONE } tac_operand_type;

typedef struct {
  tac_operand_type type;
  union {
    int immediate;
    int variable_number;
  };
} tac_operand;

typedef enum {
  TAC_OP_ADD,
  TAC_OP_SUB,
  TAC_OP_MUL,
  TAC_OP_DIV,
  TAC_OP_NEG,
  TAC_OP_ASSIGN,
} tac_operation_type;

typedef struct tac_instruction {
  tac_operation_type op;
  tac_operand result, arg1, arg2;
  struct tac_instruction *next;
} tac_instruction;

typedef struct tac_program {
  tac_instruction *head;
  tac_instruction *tail;
  int instruction_count;
} tac_program;

tac_program *tac_new(ast_node *root);
void tac_free(tac_program *program);
void tac_print(const tac_program *program);

#endif
