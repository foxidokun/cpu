#ifndef ASM_H
#define ASM_H

#include "../headers/exec.h"
#include "../file/file.h"
#include "../common/common.h"

const char VERSION = 1;
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
};

ASM_ERRORS compile (struct code_t *code, const text *source);

int translate_command (void *const buf, const char *line);

int write_binary (FILE *stream, const struct code_t *code); 


#endif // ASM_H