#ifndef BITS_H
#define BITS_H

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

struct bitflags
{
    uint64_t *words;
    size_t nwords;
};

 /**
 * @brief      Check nth bit
 *
 * @param      flags  Bit flags
 * @param[in]  index  Bit index
 *
 * @return     bool
 */
bool check_bit (const bitflags *flags, size_t index);

/**
 * @brief      Set nth bit to true
 *
 * @param      flags  Bitflags
 * @param[in]  index  Bit index
 */
void set_bit_true (bitflags *flags, size_t index);

/**
 * @brief      Set nth bit to false
 *
 * @param      flags  Bitflags
 * @param[in]  index  Bit index
 */
void set_bit_false (bitflags *flags, size_t index);

/**
 * @brief      Find value first index in flags starting from word that contains search_from to the end
 * and then from beggining to search_from word
 *
 * @param      flags  Flags
 * @param[in]  value  Value
 *
 * @return     Minimal value index or ERROR if there isn't this value in bit flags
 */
ssize_t bit_find_value (const bitflags *flags, bool value, size_t search_from);

/**
 * @brief      Creates zeroed bitflag struct
 *
 * @param[in]  capacity  Requested capacity
 *
 * @return     Pointer to allocated bitfalg structure or NULL in case of OOM
 */
bitflags *create_bitflags (size_t capacity);

/**
 * @brief      Free memory used by bitflags struct
 *
 * @param      flags  Bitflags
 */
void free_bitflags (bitflags *flags);

/**
 * @brief      Set all bitflags to false
 *
 * @param      flags  Bitflags
 */
void clear_bitflags (bitflags *flags);

#endif