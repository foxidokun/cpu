#include <assert.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "asm.h"
#include "../stack/log.h"
#include "../stack/hash.h"

static int _strcmp_void (const void *a, const void *b);

ASM_ERRORS compile (struct code_t *code, const text *source)
{
    assert (code   != nullptr && "pointer can't be null");
    assert (source != nullptr && "pointer can't be null");

    ASM_ERRORS ret = ASM_ERRORS::OK;

    for (int n_pass = 0; n_pass < NUM_OF_PASSES; ++n_pass)
    {
        code->header.code_size = 0;
        ret = one_pass_compile (code, source);
        if (ret != ASM_ERRORS::OK) return ret;

        code->n_pass++;
    }

    code->header.hash = djb2 (code->mcode, code->header.code_size);

    return ASM_ERRORS::OK;
}

ASM_ERRORS one_pass_compile (struct code_t *code, const text *source)
{
    assert (code   != nullptr && "pointer can't be null");
    assert (source != nullptr && "pointer can't be null");

    code->header.code_size = 0;

    char *mcode_ptr = (char *) code->mcode;
    ssize_t write_len = 0;
    char *line = nullptr;

    for (unsigned int i = 0; i < source->n_lines; ++i)
    {
        line = strchr (source->lines[i].content, ';');
        if (line != nullptr)
        {
            *line = '\0';
        }

        line = source->lines[i].content;
        while (isspace (*line)) line++;

        if (strlen (line) <= 1)
        {
            continue;
        }

        write_len = translate_command (mcode_ptr, line, code);
        if (write_len == ERROR)
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

    return ASM_ERRORS::OK;
}

ASM_ERRORS init_code (code_t *code)
{
    assert (code != nullptr && "pointer can't be null");

    code->pre_header.signature        = SIGNATURE;
    code->pre_header.header_version   = HEADER_VERSION;
    code->pre_header.binary_version   = BINARY_VERSION;
    code->header.hash                 = 0;
    code->header.code_size            = 0;
    code->mcode                       = calloc (CODE_BUF_RESERVED, 1);
    code->mcode_capacity              = CODE_BUF_RESERVED;
    code->n_pass                      = 0;
    if (code->mcode == nullptr) { return ASM_ERRORS::NOMEM; }

    code->name_table                  = hashmap_create (LABEL_START_CAPACITY, MAX_LABEL_LEN+1,
                                                sizeof (unsigned int), strhash, _strcmp_void);
    if (code->name_table == nullptr) { return ASM_ERRORS::NOMEM; }

    return ASM_ERRORS::OK;
}

void free_code (code_t *code)
{
    if (code == nullptr) return;

    free (code->mcode);
    hashmap_free (code->name_table);
}

#define CMD_DEF(name, number, unused, req_arg)                                          \
    if (strcasecmp (#name, cmd) == 0)                                                   \
    {                                                                                   \
        instr_ptr->opcode = number;                                                     \
        command_not_found = false;                                                      \
                                                                                        \
        ret_code = translate_arg (code, instr_ptr, line, buf_c, #name, req_arg);\
        if (ret_code == ERROR) return ERROR;                                            \
        else buf_c += ret_code;                                                         \
    } else

int translate_command (void *const buf, const char *line, code_t *code)
{
    assert (buf  != nullptr && "pointer can't be null");
    assert (line != nullptr && "pointer can't be null");
    assert (code != nullptr && "pointer can't be null");
    
    char cmd[MAX_ASM_LINE_LEN+1] = "";
    char *buf_c                  = (char *) buf;
    int cmd_len                  = 0;
    int ret_code                 = 0;
    bool command_not_found       = true;
    opcode_t *instr_ptr          = nullptr;

    ret_code = sscanf (line, "%s%n", cmd, &cmd_len);
    if (ret_code != 1) return ERROR;
    line += cmd_len;

    instr_ptr = (opcode_t *) buf_c;
    instr_ptr->m = false;
    instr_ptr->r = false;
    instr_ptr->i = false;

    //***********************************************************************//
    // IF - ELSE section (magic section)                                     //
                                                                             //
    #include "../common/opcodes.h"                                           //
                                                                             // 
    /*else*/ if (try_to_parse_label (code, line - cmd_len,                   //
                                    (int) (buf_c - (char *) code->mcode)))   //
    {                                                                        //
            command_not_found = false;                                       //
    }                                                                        //
                                                                             //
    // End of magic section                                                  //
    //***********************************************************************//

    // Not command => label

    if (command_not_found)
    {
        if (try_to_parse_label (code, line - cmd_len, (int) (buf_c - (char *) code->mcode)))
            command_not_found = false;
    }

    assert (buf_c - (char *) buf < INT_MAX && "Too long command, I'm too stupid to translate it, sorry => bad type cast");

    if (command_not_found) return ERROR;
    else                   return (int)(buf_c - (char *) buf);
}

#undef _IS_OPCODE
#undef CMD_DEF

int translate_arg (code_t *code, opcode_t *const instr_ptr, const char* asm_str, void *const buf,
                            const char *const name, const bool req_arg)
{
    assert (code      != nullptr && "pointer can't be null");
    assert (instr_ptr != nullptr && "pointer can't be null");
    assert (asm_str   != nullptr && "pointer can't be null");
    assert (buf       != nullptr && "pointer can't be null");
    assert (name      != nullptr && "pointer can't be null");

    char *buf_c  = (char *) buf + sizeof (opcode_t);
    int ret_code = 0;

    if (req_arg)
    {
        ret_code = translate_normal_arg (instr_ptr, asm_str, buf_c);
        if (ret_code == ERROR) {
            ret_code = translate_label(instr_ptr, asm_str, buf_c, code->name_table);
            if (ret_code == ERROR && code->n_pass != 0) return ERROR;

            buf_c += sizeof(int);
        } else if (strcmp (name, "pop") == 0 && instr_ptr->i && !(instr_ptr->m || instr_ptr->r)) {
            return ERROR;
        } else {
            buf_c += ret_code;
        }
    }

    assert (buf_c - (char *)buf < INT_MAX && "Too long bytecode for this opcode");
    return (int) (buf_c - (char *)buf);
}

int translate_normal_arg (opcode_t *const opcode, const char *arg_str, void *buf)
{
    assert (opcode  != nullptr && "pointer can't be null");
    assert (arg_str != nullptr && "pointer can't be null");
    assert (buf     != nullptr && "pointer can't be null");

    opcode->m       = false;
    opcode->r       = false;
    opcode->i       = false;
    int arg         = 0;
    char reg_num    = 0;
    int arg_bin_len = 0;
    bool res        = false;

    res = translate_normal_arg_imm (opcode, arg_str, &arg);
    if (res && arg == ERROR) return ERROR;

    res = translate_normal_arg_reg (opcode, arg_str, &reg_num);
    if (res && reg_num == ERROR) return ERROR;

    res = translate_normal_arg_mem (opcode, arg_str);

    if (opcode->i)
    {
        *(int *) buf = arg;
        buf = (char *) buf + sizeof (int);
        arg_bin_len += (int) sizeof (int);
    }
    if (opcode->r)
    {
        assert (reg_num <= CHAR_MAX && reg_num >= 0 && "Invalid type cast");
        *(unsigned char *) buf = (unsigned char) reg_num;
        buf = (char *) buf + sizeof (unsigned char);
        arg_bin_len += (int) sizeof (unsigned char);
    }

    if ( !(opcode->r || opcode->m || opcode->i) )
    {
        return ERROR;
    }

    return arg_bin_len;
}

bool translate_normal_arg_reg (opcode_t *const opcode, const char *arg_str, char* reg_num)
{
    assert (opcode  != nullptr && "pointer can't be null");
    assert (arg_str != nullptr && "pointer can't be null");
    assert (reg_num != nullptr && "pointer can't be null");

    const char *chr_ptr = strchr (arg_str, 'x');

    if (chr_ptr == nullptr)    return false;
    if (chr_ptr - arg_str < 2) return false;
    if (chr_ptr[-2] != 'r')    return false;
    
    *reg_num = chr_ptr[-1] - 'a';

    if      (*reg_num < 0)       *reg_num = ERROR;
    else if (*reg_num > REG_CNT) *reg_num = ERROR;

    opcode->r = true;
    return true;
}

bool translate_normal_arg_imm (opcode_t *const opcode, const char *arg_str, int* val)
{
    assert (opcode  != nullptr && "pointer can't be null");
    assert (arg_str != nullptr && "pointer can't be null");
    assert (val     != nullptr && "pointer can't be null");

    while (*arg_str != '\0' && !isdigit (*arg_str))
    {
        arg_str++;
    }

    if (*arg_str == '\0') return false;

    if (sscanf (arg_str, "%d", val) != 1)
    {
        return false;
    }
    else
    {
        opcode->i = true;
        return true;
    }
}

bool translate_normal_arg_mem (opcode_t *const opcode, const char *arg_str)
{
    assert (opcode  != nullptr && "pointer can't be null");
    assert (arg_str != nullptr && "pointer can't be null");

    while (isspace (*arg_str)) arg_str++;

    if (*arg_str != '[')
    {
        return false;
    }
    else
    {
        if (strchr(arg_str, '\0') - strrchr (arg_str, ']') != 1)
        {
            return false;
        }
        else
        {
            opcode->m = true;
            return true;
        }
    }
}

int translate_label (opcode_t *instr, const char *line, void *const buf, hashmap *name_table)
{
    assert (instr != nullptr && "pointer can't be null");
    assert (line  != nullptr && "pointer can't be null");
    assert (buf   != nullptr && "pointer can't be null");
    assert (name_table != nullptr && "pointer can't be null");

    instr->m = false;
    instr->r = false;
    instr->i = true;

    while (isspace(line[0])) line++;

    unsigned int *buf_ui  = (unsigned int *) buf;
    unsigned int *lbl_val = (unsigned int *) hashmap_get (name_table, line);

    if (lbl_val == nullptr) { *buf_ui = BAD_JMP_ADDR; return ERROR;                 }
    else                    { *buf_ui = *lbl_val;     return sizeof (unsigned int); }
}

bool try_to_parse_label (code_t *code, const char *line, int bin_pos)
{
    assert (code != nullptr && "pointer can't be null");
    assert (line != nullptr && "pointer can't be null");

    while (isspace(line[0])) line++;

    char label[MAX_LABEL_LEN+1] = "";
    strncpy (label, line, MAX_LABEL_LEN);

    int ret = 0;

    char *chr_ptr = strchr (label, ':');
    if (chr_ptr != nullptr)
    {
        *chr_ptr = '\0';
        ret = hashmap_insert (code->name_table, label, (size_t) (chr_ptr - label) + 1,
                            &bin_pos, sizeof (int));

        if (code->name_table->used == code->name_table->allocated)
        {
            code->name_table = hashmap_resize (code->name_table,
                                               code->name_table->allocated * 2);
        }

        return true;
    }

    int value = 0;
    char second_word[MAX_LABEL_LEN+1] = "";

    if (sscanf (line, "%s%s%d", label, second_word, &value) == 3)
    {
        if (strcmp ("equ", second_word) == 0)
        {
            hashmap_insert (code->name_table, label, MAX_LABEL_LEN+1,
                            &value, sizeof (int));
            return true;
        }
    }

    return false;
}

static int _strcmp_void(const void *a, const void *b)
{
    assert (a != nullptr && "pointer can't be null");
    assert (b != nullptr && "pointer can't be null");

    return strcmp ((const char *) a, (const char *) b);
}

int write_binary (FILE *stream, const struct code_t *code)
{
    assert (code   != nullptr && "pointer can't be null");
    assert (stream != nullptr && "pointer can't be null");

    size_t bin_size = code->header.code_size + sizeof (pre_header_t) + sizeof (header_t);

    size_t size_written  = fwrite (&code->pre_header, 1,  sizeof (pre_header_t), stream);
    size_written        += fwrite (&code->header,     1,  sizeof (    header_t), stream);
    size_written        += fwrite (code->mcode,       1, code->header.code_size, stream);

    if (size_written != bin_size) return ERROR;
    else                          return 0;
}