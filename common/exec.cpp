#include <assert.h>
#include "../stack/hash.h"
#include "exec.h"


BIN_ERROR verify_binary (const void *bin, size_t bin_size,
                        unsigned char bin_version, unsigned char head_version)
{
    assert (bin != nullptr);

    if (bin_size < sizeof (pre_header_t))              return BIN_ERROR::BAD_SIZE;

    if (((const pre_header_t *)bin)->signature      != SIGNATURE)    return BIN_ERROR::BAD_SIGNATURE;
    if (((const pre_header_t *)bin)->binary_version != bin_version)  return BIN_ERROR::BAD_VERSION;
    if (((const pre_header_t *)bin)->header_version != head_version) return BIN_ERROR::BAD_VERSION;

    bin = (const char *) bin + sizeof (pre_header_t);

    if (head_version == 1)
    {
        const header_v1_t *header = (const header_v1_t *) bin;
        if (bin_size != sizeof (header_v1_t) + sizeof (pre_header_t) + header->code_size)
        {
            return BIN_ERROR::BAD_SIZE;
        }

        if (djb2_hash (header+1, header->code_size) != header->hash)
        {
            return BIN_ERROR::BAD_HASH;
        }
    }
    else
    {
        assert (0 && "Invalid header version");
    }

    return BIN_ERROR::OK;
}

#define _err_msg(target_err, str)                               \
{                                                               \
    if (err == BIN_ERROR::target_err)                           \
    {                                                           \
        return str;                                             \
    }                                                           \
}
const char *bin_strerror (BIN_ERROR err)
{
    _err_msg (OK,            "Binary is ok as expected. Everthing is fine :fire:");
    _err_msg (BAD_SIGNATURE, "This code is not dead inside... Bad signature");
    _err_msg (BAD_HASH,      "This code is broken inside... Bad hash");
    _err_msg (BAD_SIZE,      "This code is broken inside... Bad size");
    _err_msg (BAD_VERSION,   "Version of binary is incomptable with current CPU. Contact /dev/null for technical support");

    assert (0 && "INVALID enum member / not all errors covered");
}
#undef _err_msg