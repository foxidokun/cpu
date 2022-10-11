#ifndef EXEC_H
#define EXEC_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

struct opcode_t {
    unsigned char opcode: OPCODE_BIT_SIZE;
    unsigned char m: 1;
    unsigned char r: 1;
    unsigned char i: 1;
};

struct pre_header_t
{
    uint64_t signature;
    unsigned char binary_version;
    unsigned char header_version;
};

struct header_v1_t
{
    uint64_t hash;
    size_t code_size;
};

enum OPCODE
{
    HLT  =   0,
    PUSH,  //1,
    POP,   //2
    ADD,   //3,
    SUB,   //4,
    DIV,   //5,
    MUL,   //6
    INC,   //7,
    DEC,   //8,
    OUT,   //9,
    INP,  //10,
    JMP,  //11
    JA,   //12
    JAE,  //13
    JB,   //14
    JBE,  //15
    JE,   //16
    JNE,  //17
    ZXC,  //18
    _OPCODE_CNT_
};

#define CMD_DEF(name, ...) #name ,

const char COMMAND_NAMES[][MAX_OPCODE_LEN+1] = {
    #include "opcodes.h"
    };

#undef CMD_DEF

enum class BIN_ERROR
{
    OK = 0,
    BAD_SIGNATURE,
    BAD_HASH,
    BAD_SIZE,
    BAD_VERSION,
};

BIN_ERROR verify_binary (const void *bin, size_t bin_size, unsigned char bin_version, unsigned char head_version);

const char *bin_strerror (BIN_ERROR err);

#endif