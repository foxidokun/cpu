#include <assert.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "asm.h"
#include "../stack/log.h"
#include "../stack/hash.h"

ASM_ERRORS compile (struct code_t *code, const text *source)
{
    assert (code   != nullptr && "pointer can't be null");
    assert (source != nullptr && "pointer can't be null");

    code->pre_header.signature = SIGNATURE;
    code->pre_header.version   = VERSION;
    code->header.hash          = 0;
    code->header.code_size     = 0;
    code->mcode                = calloc (CODE_BUF_RESERVED, 1);
    code->mcode_capacity       = CODE_BUF_RESERVED;

    if (code->mcode == nullptr) { return ASM_ERRORS::NOMEM; }

    char *mcode_ptr = (char *) code->mcode;
    ssize_t write_len = 0;

    for (unsigned int i = 0; i < source->n_lines; ++i)
    {
        if (source->lines[i].len == 0 || source->lines[i].content[0] == ';')
        {
            continue;
        }

        write_len = translate_command (mcode_ptr, source->lines[i].content);
        if (write_len == -1)
        {
            log (log::ERR, "Failed to parse command on line %d", i+1);
            return ASM_ERRORS::SYNTAX;
        }

        mcode_ptr              += (size_t) write_len;
        code->header.code_size += (size_t) write_len;

        if (code->header.code_size > code->mcode_capacity - CODE_BUF_RESERVED)
        {
            void *tmp_ptr = realloc (code->mcode, 2 * code->mcode_capacity);
            if (tmp_ptr == nullptr) { return ASM_ERRORS::NOMEM; }

            mcode_ptr = (mcode_ptr - (char *)code->mcode) + (char *)tmp_ptr;
            code->mcode = tmp_ptr;
            code->mcode_capacity *= 2;
        }
    }

    code->header.hash = djb2 (code->mcode, code->header.code_size);

    return ASM_ERRORS::OK;
}

int translate_command (void *const buf, const char *line)
{
    assert (buf  != nullptr && "pointer can't be null");
    assert (line != nullptr && "pointer can't be null");
    
    char cmd[MAX_OPCODE_LEN+1] = "";
    char *buf_c                = (char *) buf;
    int cmd_len                = 0;
    int ret_code               = 0;
    bool command_not_found     = true;

    ret_code = sscanf (line, "%s%n", cmd, &cmd_len);
    if (ret_code != 1) return ERROR;
    line += cmd_len;

    for (unsigned int i = 0; i < _OPCODE_CNT_; ++i)
    {
        if (strcasecmp (cmd, COMMAND_NAMES[i]) == 0)
        {
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wconversion"
            assert (i < 1<<OPCODE_BIT_SIZE && "Invalid instruction => overflow");
            ((opcode_t *) buf_c)->opcode = i;
            #pragma GCC diagnostic pop

            buf_c += sizeof (opcode_t);

            if (i == PUSH)
            {
                ret_code = translate_arg (((opcode_t *) buf_c) - 1, line, buf_c);
                if (ret_code == ERROR) return ERROR;
                else buf_c += ret_code;
            }

            command_not_found = false;
            break;
        }
    }

    assert (buf_c - (char *) buf < INT_MAX && "Too long command => bad type casting");

    if (command_not_found) return ERROR;
    else                   return (int)(buf_c - (char *) buf);
}

#define _SKIP_SPACE                                 \
{                                                   \
    while (isspace(arg_str[0]) && arg_len > 0)      \
    {                                               \
        arg_str++;                                  \
        arg_len--;                                  \
    }                                               \
                                                    \
}

int translate_arg (opcode_t *const opcode, const char *arg_str, void *buf)
{
    assert (opcode  != nullptr && "pointer can't be null");
    assert (arg_str != nullptr && "pointer can't be null");
    assert (buf     != nullptr && "pointer can't be null");

    opcode->m       = false;
    opcode->r       = false;
    opcode->i       = false;
    int arg         = 0;
    int reg_num     = 0;
    int arg_bin_len = 0;
    size_t arg_len  = strlen (arg_str);

    _SKIP_SPACE
    if (arg_str[0] == '[')
    {
        if (arg_str[arg_len-1] != ']') return ERROR;
        else 
        {
            opcode->m = true;
            arg_str++;
            arg_len -= 2;
        }
    }

    for (int i = 0; i < 3; ++i)
    {
        _SKIP_SPACE
        if (isdigit (arg_str[0]))
        {
            if (opcode->i) return ERROR;
            opcode->i   = true;
            int int_len = 0;

            sscanf (arg_str, "%d%n", &arg, &int_len);
            if (int_len == 0) return ERROR;        
            arg_str += int_len;
            arg_len -= int_len;
        }
        else if (arg_str[0] == 'r' && arg_str[2] == 'x')
        {
            if (opcode->r) return ERROR;
            opcode->r = true;
            reg_num   = arg_str[1] - 'a';
            arg_str  += 3;
            arg_len  -= 3;

            if (reg_num < 0 || reg_num > REG_CNT) return ERROR;
        }
        else if (arg_str[0] == '+' && i == 1)
        {
            arg_str++;
            arg_len--;
        }
        else if (arg_str[0] != '\0' && arg_len != 0)
        {
            log (log::DBG, "Failed with str '%s', char: %d", arg_str, arg_str[0]);
            return ERROR;
        }

        _SKIP_SPACE
    }

    if (opcode->i)
    {
        *(int *) buf = arg;
        buf = (char *) buf + sizeof (int);
        arg_bin_len += sizeof (int);
    }
    if (opcode->r)
    {
        *(unsigned char *) buf = reg_num;
        buf = (char *) buf + sizeof (unsigned char);
        arg_bin_len += sizeof (unsigned char);
    }

    return arg_bin_len;
}

#undef _SKIP_SPACE

int write_binary (FILE *stream, const struct code_t *code)
{
    assert (code   != nullptr && "pointer can't be null");
    assert (stream != nullptr && "pointer can't be null");

    size_t bin_size = code->header.code_size + sizeof (pre_header_t) + sizeof (header_t);

    size_t size_written  = fwrite (&code->pre_header, 1,  sizeof (pre_header_t), stream);
    size_written        += fwrite (&code->header,     1,  sizeof (    header_t), stream);
    size_written        += fwrite (code->mcode,      1, code->header.code_size, stream);

    if (size_written != bin_size) return ERROR;
    else                          return 0;
}