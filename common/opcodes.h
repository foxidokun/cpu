#ifndef CMD_DEF
#error "No CMD_DEF macros"
#endif

/// Requires CMD_DEF macros with 4 parameters: opcode name, number, code, and (bool) is argument required

#define _CMD_DEF_ONE_OP(name, number, func) \
    CMD_DEF (name, number, {                \
        POP_DATA (&OP1);                    \
        OP1 = func (OP1);                   \
        PUSH_DATA (&OP1);                   \
    }, 0)                                   \

#define _CMD_DEF_ARTHM(name, number, oper, check) \
    CMD_DEF (name, number, {                      \
        POP_DATA (&OP2);                          \
        POP_DATA (&OP1);                          \
        {check};                                  \
        OP1 = OP1 oper OP2;                       \
        PUSH_DATA (&OP1);                         \
    }, 0)                                         \

#define _CMD_DEF_JMP_IF(name, number, cond) \
    CMD_DEF (name, number, {                \
        POP_DATA (&OP2);                    \
        POP_DATA (&OP1);                    \
        OP1 = (OP1 cond OP2);               \
        OP2 = GET_ARG ();                   \
        if (OP1)                            \
        {                                   \
            JMP (OP2);                      \
        }                                   \
    }, 1)                                   \

CMD_DEF (halt, 0, { HLT (); }, 0)
CMD_DEF (push, 1, { OP1 = GET_ARG (); PUSH_DATA (&OP1); }, 1)
CMD_DEF (pop,  2, {
    OPPTR = GET_ARG_POP ();
    if (OPPTR == nullptr)
    {
        ERRLOG ("Invalid pop argument");
        SYNTAX_ERROR ();
    }
    else
    {
        POP_DATA (OPPTR);
    }
}, 1)

_CMD_DEF_ARTHM (add, 3, +, ;)
_CMD_DEF_ARTHM (sub, 4, -, ;)
_CMD_DEF_ARTHM (div, 5, /, {
    if (OP2 == 0) {
        log (log::ERR, "Zero division error");
        ZERODIV();
    }
})
_CMD_DEF_ARTHM (mul, 6, *, ;)

_CMD_DEF_ONE_OP (inc, 7,  ++)
_CMD_DEF_ONE_OP (dec, 8,  --)

CMD_DEF (out, 9, {
    POP_DATA (&OP1);
    OUT (OP1);
}, 0)

CMD_DEF (inp, 10, {
    INP  (OP1);
    PUSH_DATA (&OP1);
}, 0)

CMD_DEF (jmp, 11, {
    JMP (GET_ARG ());
}, 1)

_CMD_DEF_JMP_IF (ja,  12, > )
_CMD_DEF_JMP_IF (jae, 13, >=)
_CMD_DEF_JMP_IF (jb,  14, < )
_CMD_DEF_JMP_IF (jbe, 15, <=)
_CMD_DEF_JMP_IF (je,  16, ==)
_CMD_DEF_JMP_IF (jne, 17, !=)

_CMD_DEF_ONE_OP (zxc, 18, -7 + )

CMD_DEF (call, 19, {
    OP1 = GET_ARG ();
    PUSH_ADDR (&CURRENT_POS);
    JMP (OP1);
}, 1)

CMD_DEF (ret, 20, {
    POP_ADDR (&OP1);
    JMP (OP1);
}, 1)

CMD_DEF (dump, 21, {
    DUMP ();
}, 0)

_CMD_DEF_ONE_OP (sqrt, 22, SQRT)