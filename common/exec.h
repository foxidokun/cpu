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