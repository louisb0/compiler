#ifndef tac_h
#define tac_h

#include "ast.h"

typedef struct tac_instruction tac_instruction;
typedef struct tac_program tac_program;

tac_program *tac_new(ast_node *root);
void tac_free(tac_program *program);
void tac_print(const tac_program *program);

#endif
