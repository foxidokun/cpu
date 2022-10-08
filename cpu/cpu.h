#ifndef CPU_H
#define CPU_H

#include "../common/exec.h"
#include "../common/config.h"
#include "../stack/stack.h"

const int VERSION = 1;
typedef header_v1_t header_t;

enum class CPU_ERRORS
{
    OK      = 0,
    SYNTAX,
    NOMEM,
    INTERNAL_ERROR
};

struct cpu_t
{
    const char* code;
    stack_t stk;
    int regs[REG_CNT];
    int ram[TOTAL_RAM_SIZE];
};

CPU_ERRORS run_binary (const void *binary, size_t binary_size);

CPU_ERRORS execute (const void *code, size_t code_size);

CPU_ERRORS cpu_free (cpu_t *cpu);

CPU_ERRORS cpu_init (cpu_t *cpu, const void* code);

#endif //CPU_H