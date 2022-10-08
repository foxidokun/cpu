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

int translate_command (char *buf, const void *code, size_t *code_shift)
{
    assert (buf  != nullptr && "pointer can't be null");
    assert (code != nullptr && "pointer can't be null");

    unsigned int cnt = 0;
    int tmp_cnt      = 0;
    opcode_t cmd     = *(const opcode_t *) code;
    if (cmd.opcode >= _OPCODE_CNT_) return -1;

    strcpy (buf, COMMAND_NAMES[cmd.opcode]);
    unsigned int command_len = (unsigned int) strlen (COMMAND_NAMES[cmd.opcode]);
    buf += command_len;
    cnt += command_len;
    code = (const char *) code + sizeof (opcode_t);
    *code_shift = sizeof (opcode_t); 

    // Commands that require special actions
    switch (cmd.opcode)
    {
        case PUSH:
            sprintf (buf, " %d%n", *(const int *)code, &tmp_cnt);
            assert (tmp_cnt >= 0 && "%n less than zero");
            cnt += (unsigned) tmp_cnt;
            buf += (unsigned) tmp_cnt;
            *code_shift += sizeof (int);
            break;
        
        default:
            break;
    }

    buf[0] = '\n';
    cnt++;

    assert (cnt <= RESERVED_BUF_SIZE && "cnt > RESERVED_BUF_SIZE, possible out-of-array access");
    return (int) cnt;
}