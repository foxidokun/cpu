#ifndef CPU_H
#define CPU_H

#include <SDL2/SDL.h>

#include "../common/exec.h"
#include "../common/config.h"
#include "../stack/stack.h"

// #define ALWAYS_DUMP
#define ENABLE_VIDEO

typedef header_v1_t header_t;

enum class CPU_ERRORS
{
    OK      = 0,
    SYNTAX,
    NOMEM,
    INTERNAL_ERROR,
    ZERO_DIV
};

struct cpu_t
{
    const char* code;
    stack_t data_stk;
    stack_t addr_stk;
    int regs[REG_CNT];
    int ram[TOTAL_RAM_SIZE];
    size_t code_size;
    size_t in;
    size_t last_ram_indx;
    SDL_Renderer *renderer;
    SDL_Window *window;
};

struct color_t
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

union int_color_wrapper_t
{
    int arg;
    color_t color;
};

CPU_ERRORS run_binary (const void *binary, size_t binary_size);

CPU_ERRORS execute (cpu_t *const cpu);

CPU_ERRORS cpu_free (cpu_t *cpu);

CPU_ERRORS cpu_init (cpu_t *cpu, const void* code, size_t code_size);

void dump_cpu (cpu_t *cpu);
void render_video (cpu_t *cpu);

int  extract_arg (cpu_t *cpu, const opcode_t *const instruct);
int *extract_arg_pop (cpu_t *cpu, const opcode_t *const instruct);

#endif //CPU_H