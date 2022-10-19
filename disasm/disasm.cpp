#include <assert.h>
#include <string.h>
#include "../stack/log.h"
#include "../common/common.h"
#include "disasm.h"

DISASM_ERRORS disassembly (char **const dest, size_t *const buf_size, const void *source, size_t binary_size)
{
    assert ( dest  != nullptr && "Pointer can't be null");
    assert (source != nullptr && "Pointer can't be null");

    BIN_ERROR err = verify_binary (source, binary_size, BINARY_VERSION, HEADER_VERSION);
    if (err != BIN_ERROR::OK)
    {
        log (log::ERR, "Wrong binary, error: %s", bin_strerror (err));
        return DISASM_ERRORS::BAD_BINARY;
    }

    source = (const char *)source + sizeof (pre_header_t) + sizeof (header_t);

    int buf_shift     = 0;
    size_t buf_index  = 0;
    size_t code_shift = 0;
    size_t code_size  = ((const header_t *)source)[-1].code_size;

    while (code_size > 0)
    {
        if (buf_index + RESERVED_BUF_SIZE> *buf_size)
        {
            size_t new_buf_size = MAX (RESERVED_BUF_SIZE, 2*(*buf_size));

            void *tmp_ptr = realloc (*dest, new_buf_size);
            if (tmp_ptr == nullptr) { return DISASM_ERRORS::NOMEM; };

            *dest = (char *) tmp_ptr;
            *buf_size = new_buf_size;
        }

        buf_shift = translate_command (*dest + buf_index, source, &code_shift);
        if (buf_shift == -1)
        {
            log (log::ERR, "Wrong binary, invalid opcode");
            return DISASM_ERRORS::BAD_BINARY;
        }

        code_size -= (unsigned) code_shift;
        buf_index += (unsigned) buf_shift;
        source = (const char *) source + code_shift;
    }

    *buf_size = buf_index;
    return DISASM_ERRORS::OK;
}

#define CMD_DEF(name, number, unused, req_arg)                   \
if (cmd.opcode == number)                                        \
{                                                                \
    cnt += write_command (&cmd, buf, code, code_shift, #name, req_arg); \
} else

int translate_command (char *buf, const void *code, size_t *code_shift)
{
    assert (buf  != nullptr && "pointer can't be null");
    assert (code != nullptr && "pointer can't be null");

    unsigned int cnt = 0;
    opcode_t cmd     = *(const opcode_t *) code;

    //**************************************//
    // IF-ELSE section (define magic)       //
                                            //
    #include "../common/opcodes.h"          //
                                            //
    /*else*/ {                              //
        return -1;                          //
    }                                       //
                                            //
    // End magic section                    //
    //**************************************//

    buf[cnt] = '\n';
    cnt++;

    assert (cnt <= RESERVED_BUF_SIZE && "cnt > RESERVED_BUF_SIZE, possible out-of-array access");
    return (int) cnt;
}

unsigned int write_command (const opcode_t *instr, char *buf, const void *code, size_t *code_shift,
                            const char *const opcode_name, bool req_arg)
{
    assert (instr       != nullptr && "pointer can't be null");
    assert (buf         != nullptr && "pointer can't be null");
    assert (code        != nullptr && "pointer can't be null");
    assert (code_shift  != nullptr && "pointer can't be null");
    assert (opcode_name != nullptr && "pointer can't be null");

    unsigned int write_cnt = 0;
    unsigned int   tmp_cnt = 0;

    strcpy (buf, opcode_name);
    unsigned int  command_len = (unsigned int) strlen (opcode_name);
    write_cnt  += command_len;
    buf        += command_len;

    code = (const char *) code + sizeof (opcode_t);
    *code_shift = sizeof (opcode_t);

    if (req_arg)
    {
        buf[0] = ' ';
        buf++;
        write_cnt++;
        tmp_cnt = write_arg (instr, buf, code, code_shift);
        buf       += tmp_cnt;
        write_cnt += tmp_cnt;
    }

    return write_cnt;
}

unsigned int write_arg (const opcode_t *instr, char *buf, const void *code, size_t *code_shift)
{
    assert (instr != nullptr && "pointer can't be null");
    assert (buf   != nullptr && "pointer can't be null");
    assert (code  != nullptr && "pointer can't be null");
    assert (code_shift != nullptr && "pointer can't be null");

    char tmp_buf[MAX_ASM_LINE_LEN+1] = "";

    unsigned int write_cnt = 0;
    int tmp_cnt = 0;

    if (instr->i) {
        sprintf (tmp_buf, "%d%n", *(const int *) code, &tmp_cnt);
        assert (tmp_cnt > 0 && "Impossible %n return => bad type cast");
        write_cnt = (unsigned int) tmp_cnt;
        code = (const char*) code +  sizeof (int);
        *code_shift         += sizeof (int);
    }
    if (instr->r)
    {
        if (instr->i)
        {
            sprintf (tmp_buf+write_cnt, "+");
            write_cnt++;
        }

        sprintf (tmp_buf+write_cnt, "r%cx", 'a'+*(const unsigned char*)code);
        write_cnt   += 3;
        *code_shift += sizeof (char);
    }

    if (instr->m)
    {
        buf[0]           = '[';
        buf[write_cnt+1] = ']';
        memcpy (buf+1, tmp_buf, write_cnt);
        write_cnt += 2;
    }
    else
    {
        memcpy (buf, tmp_buf, write_cnt);
    }

    return write_cnt;
}