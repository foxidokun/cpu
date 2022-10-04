#include <stdio.h>
#include <string.h>

#include "../common/exec.h"
#include "../stack/hash.h"
#include "../file/file.h"
#include "../stack/log.h"
#include "asm.h"

int main (int argc, char *argv[])
{
    const char *out_filename = nullptr;
    set_log_level  (log::DBG);

    if ((argc == 1) || (argc == 2 && strcmp (argv[1], "-h") == 0))
    {
        fprintf (stderr, "Usage: ./asm <input file> [<output file>]\n");
        return -1;
    }
    else if (argc == 3) 
    {
        out_filename = argv[2];
    }
    else
    {
        out_filename = "a.bin";
    }

    FILE *in_file  = fopen (argv[1],      "r" );
    FILE *out_file = fopen (out_filename, "wb");
    if (in_file  == nullptr) { log(log::ERR, "Failed to open file '%s'\n", argv[1]);      return ERROR; }
    if (out_file == nullptr) { log(log::ERR, "Failed to open file '%s'\n", out_filename); return ERROR; }

    struct text *source = read_text (in_file);
    _UNWRAP_NULL_ERR (source);
    fclose (in_file);

    struct code_t code = {};
    assert (compile (&code, source) == ASM_ERRORS::OK);
    assert (write_binary (out_file, &code) == 0);
    
    fclose (out_file);
    free_text (source);
}