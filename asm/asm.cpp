#include <assert.h>
#include <string.h>
#include <limits.h>

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
    
    char *buf_c = (char *) buf;
    char cmd[MAX_OPCODE_LEN+1] = "";
    int cmd_len  = 0;
    int ret_code = 0;
    bool syntax_error = true;

    ret_code = sscanf (line, "%s%n", cmd, &cmd_len);
    if (ret_code != 1) return ERROR;
    line += cmd_len;

    for (int i = 0; i < _OPCODE_CNT_; ++i)
    {
        if (strcasecmp (cmd, COMMAND_NAMES[i]) == 0)
        {
            * ((opcode_t *) buf_c) = (opcode_t) i;
            buf_c += sizeof (opcode_t);

            if (i == PUSH)
            {
                double arg = 0.0;
                
                ret_code = sscanf (line, "%lg", &arg);
                if (ret_code != 1) return ERROR;

                * ((double *) buf_c) = arg;
                buf_c += sizeof (double);
            }

            syntax_error = false;
            break;
        }
    }

    assert (buf_c - (char *) buf < INT_MAX && "Too long command => bad type casting");

    if (syntax_error) return ERROR;
    else              return (int)(buf_c - (char *) buf);
}

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