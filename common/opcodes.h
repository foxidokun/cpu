#ifndef CMD_DEF
#error "No CMD_DEF macros"
#endif

#define _CMD_DEF_ONE_OP(name, number, func)     \
    CMD_DEF (name, number, {                    \
        POP (&OP1);                             \
        OP1 = func (OP1);                       \
        PUSH (&OP1);                            \
    })                                          \

#define _CMD_DEF_ARTHM(name, number, oper, check)   \
    CMD_DEF (name, number, {                        \
        POP (&OP2);                                 \
        POP (&OP1);                                 \
        {check};                                    \
        OP1 = OP1 oper OP2;                         \
        PUSH (&OP1);                                \
    })                                              \

#define _CMD_DEF_JMP_IF(name, number, cond)         \
    CMD_DEF (name, number, {                        \
        POP (&OP2);                                 \
        POP (&OP1);                                 \
        if (OP1 cond OP2)                           \
        {                                           \
            _JMP(GET_ARG ());                       \
        }                                           \
    })                                              \

CMD_DEF (halt,  0, { HLT; })
CMD_DEF (push, 1, { PUSH (GET_ARG    ()); })
CMD_DEF (pop,  2, { POP  (GET_ARG_POP());  })

_CMD_DEF_ARTHM (add, 3, +, ;)
_CMD_DEF_ARTHM (sub, 4, -, ;)
_CMD_DEF_ARTHM (div, 5, -, {
    if (OP2 == 0) {
        log (log::ERR, "Zero division error");
        ERR;
    }
})
_CMD_DEF_ARTHM (mul, 6, *, ;)

_CMD_DEF_ONE_OP (inc, 7,  ++)
_CMD_DEF_ONE_OP (dec, 8,  ++)

CMD_DEF (out, 9, {
    POP (&OP1);
    OUT (OP1);
})

CMD_DEF (inp, 10, {
    INP  (&OP1);
    PUSH (&OP1);
})

CMD_DEF (jmp, 11, {
    _JMP (GET_ARG ())
})

_CMD_DEF_JMP_IF (ja,  12, > )
_CMD_DEF_JMP_IF (jae, 13, >=)
_CMD_DEF_JMP_IF (jb,  14, < )
_CMD_DEF_JMP_IF (jbe, 15, <=)
_CMD_DEF_JMP_IF (je,  16, ==)
_CMD_DEF_JMP_IF (jne, 17, !=)

_CMD_DEF_ONE_OP (zxc, 18, ++)
