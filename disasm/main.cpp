#include <stdio.h>
#include <string.h>

#include "../common/exec.h"
#include "../stack/hash.h"
#include "../file/file.h"
#include "../stack/log.h"
#include "disasm.h"

int main (int argc, char *argv[])
{
    const char *out_filename = nullptr;
    set_log_level  (log::DBG);

    if ((argc == 1) || (argc == 2 && strcmp (argv[1], "-h") == 0))
    {
        fprintf (stderr, "Usage: ./disasm <input file> [<output file>]\n");
        return -1;
    }
    else if (argc == 3) out_filename = argv[2];
    else                out_filename = "a.asm";

    FILE *in_file  = fopen (argv[1],      "r" );
    FILE *out_file = fopen (out_filename, "wb");
    if (in_file  == nullptr) { log(log::ERR, "Failed to open file '%s'\n", argv[1]);      return ERROR; }
    if (out_file == nullptr) { log(log::ERR, "Failed to open file '%s'\n", out_filename); return ERROR; }

    char   *code      = read_file (in_file); _UNWRAP_NULL_ERR (code);
    ssize_t code_size = file_size (in_file);
    if (code_size == -1) { log (log::ERR, "Failed to get binary file size"); }
    
    fclose (in_file);

    char *disasm = nullptr;
    size_t disasm_size = 0;
    DISASM_ERRORS err  = disassembly (&disasm, &disasm_size, code, (size_t) code_size);

    if (disasm == nullptr) { log (log::ERR, "Binary without code"); return ERROR; }
    if (err != DISASM_ERRORS::OK)
    {
        switch (err)
        {
            case DISASM_ERRORS::NOMEM:
                log (log::ERR, "No memory for disasm buffer, buy new server please");
                return ERROR;
                break;

            case DISASM_ERRORS::BAD_BINARY:
                log (log::ERR, "Bad binary, see logs for more info");
                return ERROR;
                break;

            case DISASM_ERRORS::OK:
                assert (0 && "Impossible switch");
                break;                

            default:
                assert (0 && "Impossible switch");
                break;
        }
    }

    fwrite (disasm, disasm_size, 1, out_file);
    
    fclose (out_file);
    free (code);
    free (disasm);
}