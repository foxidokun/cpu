#include <assert.h>
#include <math.h>

#include "../common/exec.h"
#include "../stack/log.h"
#include "cpu.h"

CPU_ERRORS run_binary (const void *binary, size_t binary_size)
{
    assert (binary != nullptr && "pointer can't be null");

    BIN_ERROR res = verify_binary (binary, binary_size, BINARY_VERSION, HEADER_VERSION);
    if (res != BIN_ERROR::OK)
    {
        log (log::ERR, "Invalid binary, error: %s", bin_strerror (res));
        return CPU_ERRORS::SYNTAX;
    }

    binary = (const char *)binary + sizeof (pre_header_t) + sizeof (header_t);
    size_t code_size  = ((const header_t *)binary)[-1].code_size;

    cpu_t *cpu = (cpu_t *) calloc (1, sizeof (cpu_t));
    if (cpu == nullptr) { return CPU_ERRORS::NOMEM; }
    cpu_init (cpu, binary, code_size);

    CPU_ERRORS exec_res = execute (cpu);

    free (cpu);
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

#define _JMP_CODE()                                       \
cpu->in = (unsigned int) extract_arg_push (cpu, instruct);


#define _JMP_IF(opcode, oper)                    \
case opcode:                                     \
    _STK_UNWRAP (stack_pop (&cpu->stk, &op2));   \
    _STK_UNWRAP (stack_pop (&cpu->stk, &op1));   \
    if (op1 oper op2)                            \
    {                                            \
        _JMP_CODE()                              \
    }                                            \
    else                                         \
    {                                            \
        cpu->in += sizeof (int);                 \
    }                                            \
    break;

CPU_ERRORS execute (cpu_t *const cpu)
{
    assert (cpu != nullptr && "pointer can't be null");

    const opcode_t *instruct = nullptr;
    int op1 = 0, op2 = 0;
    int *op_ptr;

    for (cpu->in = 0; cpu->in < cpu->code_size;)
    {
        instruct = (const opcode_t *) (cpu->code + cpu->in);
        cpu->in += sizeof (opcode_t);

        log (log::DBG, "Decoding opcode %2d (%*s)", instruct->opcode, MAX_OPCODE_LEN, COMMAND_NAMES[instruct->opcode]);

        switch (instruct->opcode) {
            case HLT:
                return CPU_ERRORS::OK;
                break;

            case PUSH:
                op1 = extract_arg_push (cpu, instruct);
                _STK_UNWRAP (stack_push (&cpu->stk, &op1));
                break;

            case POP:
                op_ptr = extract_arg_pop (cpu, instruct);
                if (op_ptr == nullptr)
                {
                    log (log::ERR, "Invalid pop argument");
                    return CPU_ERRORS::SYNTAX;
                }
                _STK_UNWRAP (stack_pop (&cpu->stk, op_ptr));
                break;

            _ARTHM(ADD, +)
            _ARTHM(SUB, -)
            _ARTHM(DIV, /)
            _ARTHM(MUL, *)

            _ONE_EL_FUNC(INC, ++);
            _ONE_EL_FUNC(DEC, --);
            _ONE_EL_FUNC(ZXC, -7 +);
            
            case JMP:
                _JMP_CODE();
                break;

            _JMP_IF (JA,  > );
            _JMP_IF (JAE, >=);
            _JMP_IF (JB,  < );
            _JMP_IF (JBE, <=);
            _JMP_IF (JE,  ==);
            _JMP_IF (JNE, !=);

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

int extract_arg_push (cpu_t *cpu, const opcode_t *const instruct)
{
    assert (cpu      != nullptr && "pointer can't be null");
    assert (instruct != nullptr && "pointer can't be null");
    
    int arg = 0;

    if (instruct->i)
    {
        arg += *(const int *) (cpu->code + cpu->in);
        cpu->in += sizeof (int);
    }
    if (instruct->r)
    {
        arg += cpu->regs[((const unsigned char *) cpu->code)[cpu->in]];
        cpu->in += sizeof (unsigned char);
    }
    if (instruct->m)
    {
        arg = cpu->ram[arg];
    }

    return arg;
}

int *extract_arg_pop (cpu_t *cpu, const opcode_t *const instruct)
{
    assert (cpu      != nullptr && "pointer can't be null");
    assert (instruct != nullptr && "pointer can't be null");
    
    int *arg_ptr = nullptr;
    opcode_t instruct_copy = *instruct;

    if (instruct -> m)
    {
        instruct_copy.m = false;
        arg_ptr = &cpu->ram[extract_arg_push (cpu, &instruct_copy)];
        instruct_copy.m = true;
    }
    else
    {
        if (instruct->i) return nullptr;
        
        arg_ptr = &cpu->regs[((const unsigned char *) cpu->code)[cpu->in]];
        cpu->in += sizeof (unsigned char);
    }

    return arg_ptr;
}

CPU_ERRORS cpu_init (cpu_t *cpu, const void* code, size_t code_size)
{
    assert (cpu  != nullptr && "pointer can't be null");
    assert (code != nullptr && "pointer can't be null");

    cpu->code      = (const char *) code;
    cpu->code_size = code_size;
    cpu->in        = 0;
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