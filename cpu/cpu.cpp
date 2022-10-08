#include <assert.h>
#include <math.h>

#include "../common/exec.h"
#include "../stack/log.h"
#include "cpu.h"

CPU_ERRORS run_binary (const void *binary, size_t binary_size)
{
    assert (binary != nullptr && "pointer can't be null");

    BIN_ERROR res = verify_binary (binary, binary_size, VERSION);
    if (res != BIN_ERROR::OK)
    {
        log (log::ERR, "Invalid binary, error: %s", bin_strerror (res));
        return CPU_ERRORS::SYNTAX;
    }

    binary = (const char *)binary + sizeof (pre_header_t) + sizeof (header_t);
    size_t code_size  = ((const header_t *)binary)[-1].code_size;

    CPU_ERRORS exec_res = execute (binary, code_size);

    return exec_res;
}

#define _STK_UNWRAP(expr)                                                               \
{                                                                                       \
    err_flags _stack_res = expr;                                                        \
    if (_stack_res != res::OK)                                                          \
    {                                                                                   \
        if (_stack_res == res::EMPTY)                                                   \
        {                                                                               \
            log (log::ERR, "Syntax error: not enough data in stack for operation");     \
            return CPU_ERRORS::SYNTAX;                                                  \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            log (log::ERR, "Internal stack errors:");                                   \
            stack_perror (_stack_res, get_log_stream(), "\t->");                        \
            return CPU_ERRORS::INTERNAL_ERROR;                                          \
        }                                                                               \
    }                                                                                   \
}

#define _ARTHM(opcode, oper)                     \
case opcode:                                     \
    _STK_UNWRAP (stack_pop (&cpu->stk, &op2));   \
    _STK_UNWRAP (stack_pop (&cpu->stk, &op1));   \
    op1 = op1 oper op2;                          \
    _STK_UNWRAP (stack_push(&cpu->stk, &op1));   \
    break;                                       \

#define _ONE_EL_FUNC(opcode, func)               \
case opcode:                                     \
    _STK_UNWRAP (stack_pop (&cpu->stk, &op1));   \
    op1 = func (op1);                            \
    _STK_UNWRAP (stack_push(&cpu->stk, &op1));   \
    break; 

CPU_ERRORS execute (const void *code, size_t code_size)
{
    assert (code != nullptr && "pointer can't be null");

    int cmd = 0, op1 = 0, op2 = 0;
    bool m_flag = false, r_flag = false, i_flag = false;

    cpu_t *cpu = (cpu_t *) calloc (1, sizeof (cpu_t));
    if (cpu == nullptr) { return CPU_ERRORS::NOMEM; }
    cpu_init (cpu, code);

    while (code_size > 0)
    {
        cmd = ((const opcode_t *) cpu->code)->opcode;
        m_flag   = ((const opcode_t *) code)->m;
        r_flag   = ((const opcode_t *) code)->r;
        i_flag   = ((const opcode_t *) code)->i;
        cpu->code += sizeof (opcode_t);
        code_size -= sizeof (opcode_t);

        log (log::DBG, "Decoding opcode %d (%s)", cmd, COMMAND_NAMES[cmd]);

        switch (cmd) {
            case HLT:
                free (cpu);
                return CPU_ERRORS::OK;
                break;

            case PUSH:
                _STK_UNWRAP (stack_push (&cpu->stk, cpu->code));
                cpu->code += sizeof (int);
                code_size -= sizeof (int);
                break;

            _ARTHM(ADD, +)
            _ARTHM(SUB, -)
            _ARTHM(DIV, /)
            _ARTHM(MUL, *)

            _ONE_EL_FUNC(INC, ++);
            _ONE_EL_FUNC(DEC, --);
            
            case OUT:
                _STK_UNWRAP (stack_pop (&cpu->stk, &op1));
                printf ("you really can't calculate %i without calc?\n", op1);
                break;

            case INP:
    
                printf ("Stupid programmer decided to ask you for a number at runtime: \n");
                scanf ("%i", &op1);
                _STK_UNWRAP (stack_push (&cpu->stk, &op1));
                break;

            default:
                assert (0 && "Imposible route");
        }
    }

    log (log::WRN, "No halt opcode in binary");
    return CPU_ERRORS::OK;
}

CPU_ERRORS cpu_init (cpu_t *cpu, const void* code)
{
    assert (cpu  != nullptr && "pointer can't be null");
    assert (code != nullptr && "pointer can't be null");

    cpu->code = (const char *) code;
    stack_ctor (&cpu->stk, sizeof (int));

    return CPU_ERRORS::OK;
}

CPU_ERRORS cpu_free (cpu_t *cpu)
{
    assert (cpu != nullptr && "pointer can't be null");

    err_flags dtor_res = stack_dtor (&cpu->stk);
    if (dtor_res != res::OK)
    {
        log (log::ERR, "Failed to destruct internal stack. Stack errors:");
        stack_perror (dtor_res, get_log_stream(), "\t\t-->");
        return CPU_ERRORS::INTERNAL_ERROR;
    }

    return CPU_ERRORS::OK;
}

#undef _ARTHM
#undef _TWO_EL_FUNC
#undef _ONE_EL_FUNC