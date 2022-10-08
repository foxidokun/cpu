#include "../common/common.h"
#include "bits.h"

static const int EXP_BIT_IN_WORD = 6; // 2^6= 64
static const int NUM_BIT_IN_WORD = (1 << EXP_BIT_IN_WORD);

bool check_bit (const bitflags *flags, size_t index)
{
    assert (flags != NULL && "pointer can't be NULL");

    unsigned char nbit = index & (NUM_BIT_IN_WORD - 1); // index % 64
    index = index >> EXP_BIT_IN_WORD; // index = index / 64

    return (1lu<<nbit) & flags->words[index] ? 1 : 0;
}

void set_bit_true (bitflags *flags, size_t index)
{
    assert (flags != NULL && "pointer can't be NULL");


    unsigned char nbit = index & (NUM_BIT_IN_WORD-1); // index % 64
    index >>= EXP_BIT_IN_WORD; // index = index / 64

    flags->words[index] |= (1lu<<nbit);
}

void set_bit_false (bitflags *flags, size_t index)
{
    assert (flags != NULL && "pointer can't be NULL");

    unsigned char nbit = index & (NUM_BIT_IN_WORD - 1); // index % 64
    index >>= EXP_BIT_IN_WORD; // index = index / 64

    // -1 ^ (1<<nbit)
    flags->words[index] &= -1lu ^ (1lu<<nbit);
}

ssize_t bit_find_value (const bitflags *flags, bool value, size_t search_from)
{
    assert (flags != NULL && "pointer can't be NULL");

    uint64_t *const words   = flags->words;
    uint64_t bad_word       = value ? 0 : ~0lu;
    size_t bit_index        = 0;
    size_t max_search       = flags->nwords;
    search_from = search_from >> EXP_BIT_IN_WORD;

    for (size_t i = search_from; i <= max_search; ++i)
    {
        if (i == flags->nwords)
        {
            i = 0;
            max_search = search_from;
        }

        if (words[i] != bad_word)
        {
            for (size_t bit_n = 0; bit_n < NUM_BIT_IN_WORD; ++bit_n)
            {
                bit_index = (i<<EXP_BIT_IN_WORD)+bit_n;  // i*64 + bit_index

                if (value == check_bit (flags, bit_index))
                {
                    return (ssize_t) bit_index;  
                }
            }

            assert (0 && "Impossible: Byte is 'good', but failed to find 'good' bit");
        }
    }

    return ERROR;
}

bitflags *create_bitflags (size_t capacity)
{
    bitflags *flags = (bitflags *) calloc (1, sizeof (bitflags));
    _UNWRAP_NULL (flags);

    flags->nwords = (capacity+NUM_BIT_IN_WORD-1)>>EXP_BIT_IN_WORD; // ceil (capacity/64)
    flags->words  = (uint64_t *) calloc (flags->nwords, sizeof (uint64_t));
    _UNWRAP_NULL (flags->words);

    return flags;
}

void free_bitflags (bitflags *flags)
{
    assert (flags != NULL && "pointer can't be NULL");

    free (flags->words);
    free (flags);
}

void clear_bitflags (bitflags *flags)
{
    assert (flags != NULL && "pointer can't be NULL");

    for (size_t i = 0; i < flags->nwords; ++i)
    {
        flags->words[i] = 0u;
    }
}