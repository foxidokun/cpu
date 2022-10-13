#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../common/common.h"
#include "../stack/log.h"
#include "../file/file.h"
#include "cpu.h"

int main (int argc, char *argv[])
{
    set_log_level  (log::INF);

    if ((argc != 2) || (argc == 2 && strcmp (argv[1], "-h") == 0))
    {
        fprintf (stderr, "Usage: ./cpu <input file>\n");
        return -1;
    }
    
    FILE *in_file  = fopen (argv[1], "r" );
    if (in_file  == nullptr) { log(log::ERR, "Failed to open file '%s'\n", argv[1]); return ERROR; }

    void *binary = read_file (in_file);
    ssize_t binary_size = file_size (in_file);
    if (binary_size == -1) { log (log::ERR, "Failed to get binary file size"); }
    
    fclose (in_file);

    CPU_ERRORS res = run_binary (binary, (size_t) binary_size);
    if (res != CPU_ERRORS::OK)
    {
        log (log::ERR, "CPU exited with error %d", res);
        free (binary);
        return ERROR;
    }

    free (binary);
}