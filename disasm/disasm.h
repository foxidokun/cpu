#ifndef DISASM_H
#define DISASM_H

#include "../common/exec.h"
#include "../file/file.h"
#include "../common/common.h"

const char BINARY_VERSION = 6;
const char HEADER_VERSION = 1;
typedef header_v1_t header_t;

const int RESERVED_BUF_SIZE = (MAX_OPCODE_LEN + 1 + sizeof ("2147483647") + 1) + 7; // command + " " + int + \n + reserve

enum class DISASM_ERRORS
{
    OK      = 0,
    NOMEM,
    BAD_BINARY,
};

DISASM_ERRORS disassembly (char **out, size_t *buf_size, const void *source, size_t code_size);

int translate_command (char *buf, const void *code, size_t *code_shift);

unsigned int write_arg (const opcode_t *instr, char *buf, const void *code, size_t *code_shift);

unsigned int write_command (const opcode_t *instr, char *buf, const void *code, size_t *code_shift,
                            const char *const opcode_name, bool req_arg);
#endif