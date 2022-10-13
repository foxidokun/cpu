#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdlib.h>

const int OPCODE_BIT_SIZE = 5;
const int REG_CNT         = 8;

const uint64_t SIGNATURE           = 0x1000DEAD7;
const size_t MAX_OPCODE_LEN        = 4;
const size_t MAX_LABEL_LEN         = 30 + 1; // 30 for word and one for ':'
const size_t MAX_ASM_LINE_LEN      = 35;
const size_t LABEL_START_CAPACITY  = 10;

const unsigned int BAD_JMP_ADDR = -1u;

const int VRAM_WIDTH       = 100;
const int VRAM_HEIGHT      = 100;
const int TOTAL_RAM_SIZE   = VRAM_WIDTH * VRAM_HEIGHT;

#endif