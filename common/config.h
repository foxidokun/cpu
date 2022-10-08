#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdlib.h>

const int OPCODE_BIT_SIZE = 5;
const int REG_CNT         = 8;

const uint64_t SIGNATURE = 0x1000DEAD7;
const size_t MAX_OPCODE_LEN = 4;

const int PURE_RAM_SIZE    = 100;
const int VRAM_WIDTH       = 100;
const int VRAM_HEIGHT      = 100;
const int TOTAL_RAM_SIZE   = PURE_RAM_SIZE + VRAM_HEIGHT*VRAM_WIDTH;

#endif