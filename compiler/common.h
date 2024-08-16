#ifndef common_h
#define common_h

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ERROR_OUT()                                                            \
  do {                                                                         \
    printf("Error at line %d in %s.\n", __LINE__, __FILE__);                   \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#define UNREACHABLE() assert(!"Unreachable")

#endif
