#ifndef ASM_H
#define ASM_H

#include "../common/exec.h"
#include "../file/file.h"
#include "../common/common.h"
#include "../hashmap/hashmap.h"

const char BINARY_VERSION = 4;
const char HEADER_VERSION = 1;
const int NUM_OF_PASSES   = 2;

typedef header_v1_t header_t;
const int CODE_BUF_RESERVED = 2 * (sizeof (opcode_t) + sizeof (double));

enum class ASM_ERRORS
{
    OK      = 0,
    SYNTAX,
    NOMEM,
};

struct code_t
{
    pre_header_t pre_header;
    header_t  header;
    void *mcode;
    size_t mcode_capacity;
    hashmap *name_table;
    unsigned int n_pass;
};

ASM_ERRORS init_code (code_t *code);

void free_code (code_t *code);

ASM_ERRORS compile (struct code_t *code, const text *source);

ASM_ERRORS one_pass_compile (struct code_t *code, const text *source);

int translate_command (void *const buf, const char *line, code_t *code);

int translate_arg (code_t *code, opcode_t *const instr_ptr, const char* asm_str, void *const buf,
                           const char *const name, const bool req_arg);

int translate_normal_arg (opcode_t *const opcode, const char *arg_str, void *buf);

int translate_label (opcode_t *instr, const char *line, void *const buf, hashmap *name_table);

int write_binary (FILE *stream, const struct code_t *code);

bool try_to_parse_label (code_t *code, const char *line, int bin_pos);

#endif // ASM_H