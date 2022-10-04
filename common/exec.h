#ifndef EXEC_H
#define EXEC_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char opcode_t;

const uint64_t SIGNATURE = 0x1000DEAD7;
const size_t MAX_OPCODE_LEN = 4;

struct pre_header_t
{
    uint64_t signature;
    unsigned char version;
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
    ADD,   //2,
    SUB,   //3,
    DIV,   //4,
    POW,   //5
    SIN,   //6,
    INC,   //7,
    DEC,   //8,
    OUT,   //9,
    INP,  //10,
    _OPCODE_CNT_
};

const char COMMAND_NAMES[OPCODE::_OPCODE_CNT_][MAX_OPCODE_LEN+1] = {
    "halt",
    "push",
    "add",
    "sub",
    "div",
    "pow",
    "sin",
    "inc",
    "dec",
    "out",
    "inp"
    };

enum class BIN_ERROR
{
    OK = 0,
    BAD_SIGNATURE,
    BAD_HASH,
    BAD_SIZE,
    BAD_VERSION,
};

BIN_ERROR verify_binary (const void *bin, size_t bin_size, unsigned char version);

const char *bin_strerror (BIN_ERROR err);

#endif