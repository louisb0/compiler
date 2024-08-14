#ifndef asm_h
#define asm_h

#include "tac.h"

typedef struct asm_node asm_node;

typedef enum {
  ASM_PROGRAM,
  ASM_MOV,
  ASM_UNARY_NEG,
  ASM_BINARY_ADD,
  ASM_BINARY_SUB,
  ASM_BINARY_MULT,
  ASM_BINARY_DIV,
  ASM_IDIV,
  ASM_CDQ,
  ASM_ALLOCATE_STACK,
  ASM_RET
} asm_node_type;

typedef enum {
  OPERAND_IMM,
  OPERAND_REG,
  OPERAND_PSEUDO_VAR,
  OPERAND_STACK
} operand_type;

typedef enum { REG_RAX, REG_RDX, REG_R10, REG_R11 } reg;

// OPERANDS
typedef struct {
  int value;
} operand_imm;

typedef struct {
  reg reg;
} operand_reg;

typedef struct {
  int variable_number;
} operand_pseudo_var;

typedef struct {
  int offset;
  // int old_variable_number;
} operand_stack;

typedef struct {
  operand_type type;
  union {
    operand_imm imm;
    operand_reg reg;
    operand_pseudo_var pseudo;
    operand_stack stack;
  } as;
} operand;

// INSTRUCTIONS
typedef struct {
  asm_node *next;
} asm_program;

typedef struct {
  operand src;
  operand dst;
} asm_mov;

// !!!! NEED OUR TYPES
typedef struct {
  operand operand;
} asm_unary;

// !!!! NEED OUR TYPES
typedef struct {
  operand left;
  operand right;
} asm_binary;

typedef struct {
  operand operand;
} asm_idiv;

typedef struct {
  int size;
} asm_allocate_stack;

// ASM NODE
struct asm_node {
  asm_node_type type;
  asm_node *next;

  union {
    asm_program program;
    asm_mov mov;
    asm_unary unary;
    asm_binary binary;
    asm_idiv idiv;
    asm_allocate_stack allocate_stack;
  } as;
};

asm_node *asm_new(tac_program *program);
void asm_free(asm_node **root);
void asm_print(asm_node *root);

asm_node *asm_new_mov(operand src, operand dst);
asm_node *asm_new_unary(operand operand);
asm_node *asm_new_binary(asm_node_type type, operand left, operand right);
asm_node *asm_new_idiv(operand operand);
asm_node *asm_new_cdq(void);
asm_node *asm_new_allocate_stack(int size);
asm_node *asm_new_ret(void);

#endif
