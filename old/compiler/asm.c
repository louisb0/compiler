#include "asm.h"
#include "common.h"
#include "tac.h"

static asm_node *asm_generate_program(tac_instruction *program);
static void asm_replace_pseudos(asm_node *root, int addresses[],
                                int *allocations);
static asm_node *asm_fix_allocations(asm_node *root);

asm_node *asm_new(tac_program *program) {
  asm_node *root = asm_generate_program(program->head);

  int addresses[100];
  for (int i = 0; i < 100; i++)
    addresses[i] = -1;
  int allocations = 0;
  asm_replace_pseudos(root, addresses, &allocations);

  asm_node *asm_stack_allocation =
      (asm_node *)malloc(sizeof(*asm_stack_allocation));
  asm_stack_allocation->type = ASM_ALLOCATE_STACK;
  asm_stack_allocation->as.allocate_stack.size = 4 * allocations;
  asm_stack_allocation->next = root;
  root = asm_stack_allocation;

  root = asm_fix_allocations(root);

  return root;
}

static asm_node *asm_generate_program(tac_instruction *instruction) {
  assert(instruction);

  asm_node *new;

  switch (instruction->op) {
  case TAC_OP_ADD: {
    // t1 < add t2, t3
    operand arg1 = {.type = OPERAND_PSEUDO_VAR,
                    .as = {.pseudo = {instruction->arg1.variable_number}}};
    operand arg2 = {.type = OPERAND_PSEUDO_VAR,
                    .as = {.pseudo = {instruction->arg2.variable_number}}};
    operand dest = {.type = OPERAND_PSEUDO_VAR,
                    .as = {.pseudo = {instruction->result.variable_number}}};

    asm_node *mov = (asm_node *)malloc(sizeof(*mov));
    mov->type = ASM_MOV;
    mov->as.mov.src = arg1;
    mov->as.mov.dst = dest;

    asm_node *add = (asm_node *)malloc(sizeof(*add));
    add->type = ASM_BINARY_ADD;
    add->as.binary.left = arg2;
    add->as.binary.right = dest;

    mov->next = add;

    new = mov;

    if (instruction->next)
      add->next = asm_generate_program(instruction->next);
    break;
  }
  case TAC_OP_ASSIGN: {
    // t1 <- 1

    operand arg1 = {.type = OPERAND_IMM,
                    .as = {.imm = {instruction->arg1.immediate}}};

    operand dest = {.type = OPERAND_PSEUDO_VAR,
                    .as = {.pseudo = {instruction->result.variable_number}}};

    asm_node *mov = (asm_node *)malloc(sizeof(*mov));
    mov->type = ASM_MOV;
    mov->as.mov.src = arg1;
    mov->as.mov.dst = dest;

    new = mov;
    if (instruction->next)
      mov->next = asm_generate_program(instruction->next);
    break;
  }
    // case TAC_OP_SUB:
    //   break;
    //
    // case TAC_OP_MUL:
    //   break;
    //
    // case TAC_OP_DIV:
    //   break;
    //
    // case TAC_OP_NEG:
    //   break;

  default:
    UNREACHABLE();
    break;
  }

  return new;
}

static void asm_replace_pseudos(asm_node *root, int addresses[],
                                int *allocations) {
  assert(root);

  switch (root->type) {
  case ASM_MOV: {
    operand src = root->as.mov.src;
    operand dst = root->as.mov.dst;

    if (src.type == OPERAND_PSEUDO_VAR &&
        addresses[src.as.pseudo.variable_number] == -1) {
      (*allocations)++;
      addresses[src.as.pseudo.variable_number] = (*allocations) * 4;
      operand src_new = {.type = OPERAND_STACK,
                         .as = {.stack = {
                                    .offset = (*allocations) * 4,
                                }}};
      root->as.mov.src = src_new;
    } else if (src.type == OPERAND_PSEUDO_VAR) {

      int src_stack_address = addresses[src.as.pseudo.variable_number];
      operand src_new = {.type = OPERAND_STACK,
                         .as = {.stack = {.offset = src_stack_address}}};
      root->as.mov.src = src_new;
    }

    if (dst.type == OPERAND_PSEUDO_VAR &&
        addresses[src.as.pseudo.variable_number == -1]) {
      (*allocations)++;

      addresses[dst.as.pseudo.variable_number] = (*allocations) * 4;
      operand dst_new = {.type = OPERAND_STACK,
                         .as = {.stack = {
                                    .offset = (*allocations) * 4,
                                }}};
      root->as.mov.dst = dst_new;
    } else if (dst.type == OPERAND_PSEUDO_VAR) {

      int dst_stack_address = addresses[dst.as.pseudo.variable_number];
      operand dst_new = {.type = OPERAND_STACK,
                         .as = {.stack = {.offset = dst_stack_address}}};
      root->as.mov.dst = dst_new;
    }
    break;
  }
  case ASM_BINARY_ADD: {
    operand left = root->as.binary.left;
    operand right = root->as.binary.right;

    assert(left.type == OPERAND_PSEUDO_VAR && right.type == OPERAND_PSEUDO_VAR);

    int left_stack_address = addresses[left.as.pseudo.variable_number];
    operand left_new = {.type = OPERAND_STACK,
                        .as = {.stack = {.offset = left_stack_address}}};

    int right_stack_address = addresses[right.as.pseudo.variable_number];
    operand right_new = {.type = OPERAND_STACK,
                         .as = {.stack = {.offset = right_stack_address}}};

    root->as.binary.left = left_new;
    root->as.binary.right = right_new;
    break;
  }
  default:
    UNREACHABLE();
    break;
  }

  if (root->next)
    asm_replace_pseudos(root->next, addresses, allocations);
}

static asm_node *asm_fix_allocations(asm_node *root) {
  // move or add with both OPERAND_STACK

  // mov src, dst
  // =>
  // mov src, %r10d
  // mov %r10d, dst
  //
  if (root == NULL)
    return NULL;

  switch (root->type) {
  case ASM_ALLOCATE_STACK:
    root->next = asm_fix_allocations(root->next);
    return root;
  case ASM_MOV: {
    operand src = root->as.mov.src;
    operand dst = root->as.mov.dst;

    if (src.type == OPERAND_STACK && dst.type == OPERAND_STACK) {
      operand intermediate_register = {.type = OPERAND_REG,
                                       .as = {.reg = {.reg = REG_R10}}};

      asm_node *new1 = (asm_node *)malloc(sizeof(*new1));
      new1->type = ASM_MOV;
      new1->as.mov.src = src;
      new1->as.mov.dst = intermediate_register;

      asm_node *new2 = (asm_node *)malloc(sizeof(*new2));
      new2->type = ASM_MOV;
      new2->as.mov.dst = dst;
      new2->as.mov.src = intermediate_register;

      new1->next = new2;
      new2->next = asm_fix_allocations(root->next);
      // TODO: frees here baby on the root u got rid of it....

      return new1;
    }
    root->next = asm_fix_allocations(root->next);
    return root;
  }
  case ASM_BINARY_ADD: {
    //
    // add left, right
    // =>
    // mov left, %r11d
    // add %r11d, right
    operand left = root->as.binary.left;
    operand right = root->as.binary.right;

    if (left.type == OPERAND_STACK && right.type == OPERAND_STACK) {
      operand intermediate_register = {.type = OPERAND_REG,
                                       .as = {.reg = {.reg = REG_R10}}};

      asm_node *new1 = (asm_node *)malloc(sizeof(*new1));
      new1->type = ASM_MOV;
      new1->as.mov.src = root->as.binary.left;
      new1->as.mov.dst = intermediate_register;

      asm_node *new2 = (asm_node *)malloc(sizeof(*new2));
      new2->type = ASM_BINARY_ADD;
      new2->as.binary.left = intermediate_register;
      new2->as.binary.right = root->as.binary.right;

      new1->next = new2;
      new2->next = asm_fix_allocations(root->next);
      // TODO: frees here baby on the root u got rid of it....

      return new1;
    }
    root->next = asm_fix_allocations(root->next);
    return root;
  }
  default:
    UNREACHABLE();
    return NULL;
  }
}

const char *reg_to_str(reg r) {
  switch (r) {
  case REG_RAX:
    return "%rax";
  case REG_RDX:
    return "%rdx";
  case REG_R10:
    return "%r10d";
  case REG_R11:
    return "%r11d";
  default:
    return "unknown";
  }
}

void asm_free(asm_node **root);
void asm_print(asm_node *root) {
  assert(root);

  switch (root->type) {
  case ASM_ALLOCATE_STACK:
    printf("pushq %%rbp\n");
    printf("movq %%rsp, %%rbp\n");
    printf("subq $%d, %%rsp\n", root->as.allocate_stack.size);
    asm_print(root->next);
    break;
  case ASM_MOV:
    if (root->as.mov.src.type == OPERAND_IMM &&
        root->as.mov.dst.type == OPERAND_STACK) {
      printf("movl $%d, -%d(%%rbp)\n", root->as.mov.src.as.imm.value,
             root->as.mov.dst.as.stack.offset);
    } else if (root->as.mov.src.type == OPERAND_STACK &&
               root->as.mov.dst.type == OPERAND_REG) {
      printf("movl -%d(%%rbp), %s\n", root->as.mov.src.as.stack.offset,
             reg_to_str(root->as.mov.dst.as.reg.reg));
    } else if (root->as.mov.src.type == OPERAND_REG &&
               root->as.mov.dst.type == OPERAND_STACK) {
      printf("movl %s, -%d(%%rbp)\n", reg_to_str(root->as.mov.src.as.reg.reg),
             root->as.mov.dst.as.stack.offset);
    } else {
      UNREACHABLE();
    }

    if (root->next)
      asm_print(root->next);
    break;

  case ASM_BINARY_ADD:
    if (root->as.binary.left.type == OPERAND_REG &&
        root->as.binary.right.type == OPERAND_STACK) {
      printf("add %s, -%d(%%rbp)\n",
             reg_to_str(root->as.binary.left.as.reg.reg),
             root->as.binary.right.as.stack.offset);
    } else {
      UNREACHABLE();
    }

    if (root->next)
      asm_print(root->next);
    break;
  case ASM_PROGRAM:
    asm_print(root->as.program.next); // TODO: garbage
    break;
  default:
    UNREACHABLE();
  }
}

asm_node *asm_new_mov(operand src, operand dst);
asm_node *asm_new_unary(operand operand);
asm_node *asm_new_binary(asm_node_type type, operand left, operand right);
asm_node *asm_new_idiv(operand operand);
asm_node *asm_new_cdq(void);
asm_node *asm_new_allocate_stack(int size);
asm_node *asm_new_ret(void);
