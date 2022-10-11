#ifndef CLEANUP_DEFINES

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

#define CMD_DEF(name, number, code, ...)        \
case number:                                    \
    log (log::DBG, "Decoding opcode %2d (%*s)", \
        number, MAX_OPCODE_LEN, #name);         \
    {code};                                     \
    break;

#define OP1         op1
#define OP2         op2
#define OPPTR       op_ptr
#define CURRENT_POS cpu->in

#define POP_DATA(arg)  _STK_UNWRAP(stack_pop  (&cpu->data_stk, arg))
#define POP_ADDR(arg)  _STK_UNWRAP(stack_pop  (&cpu->addr_stk, arg))
#define PUSH_DATA(arg) _STK_UNWRAP(stack_push (&cpu->data_stk, arg))
#define PUSH_ADDR(arg) _STK_UNWRAP(stack_push (&cpu->addr_stk, arg))

#define GET_ARG()     extract_arg     (cpu, instruct)
#define GET_ARG_POP() extract_arg_pop (cpu, instruct)

#define JMP(arg) cpu->in = (unsigned int) arg

#define ZERODIV()      return CPU_ERRORS::ZERO_DIV
#define HLT()          return CPU_ERRORS::OK
#define SYNTAX_ERROR() return CPU_ERRORS::SYNTAX

#define INP(arg) printf ("Stupid programmer decided to ask you for a number at runtime: \n"); scanf ("%i", &arg)
#define OUT(arg) printf ("you really can't calculate %i without calc?\n", arg)

#define ERRLOG(fmt, ...) log (log::ERR, fmt, ##__VA_ARGS__)

#else

#undef _STK_UNWRAP
#undef CMD_DEF

#undef OP1
#undef OP2
#undef POP
#undef PUSH
#undef GET_ARG
#undef GET_ARG_POP
#undef JMP
#undef ZERODIV
#undef HLT
#undef SYNTAX_ERROR
#undef INP
#undef OUT
#undef ERRLOG

#endif