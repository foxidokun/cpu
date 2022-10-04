#ifndef CPU_H
#define CPU_H

#include "../common/exec.h"

const int VERSION = 1;
typedef header_v1_t header_t;

enum class CPU_ERRORS
{
    OK      = 0,
    SYNTAX,
    NOMEM,
    INTERNAL_ERROR
};

CPU_ERRORS run_binary (const void *binary, size_t binary_size);

CPU_ERRORS execute (const void *code, size_t code_size);

#endif //CPU_H