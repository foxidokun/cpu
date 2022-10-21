#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cpu.h"
#include "../common/common.h"
#include "../stack/log.h"

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

    char *binary = load_binary (in_file);
    fclose (in_file);

    cpu_t cpu = {};
    cpu_ctor (&cpu, binary);

    CPU_ERRORS exec_res = execute (&cpu);
    if (exec_res != CPU_ERRORS::OK)
    {
        log (log::ERR, "Failed tp execute with error code %d", exec_res);
    }

    free (binary);
    cpu_dtor (&cpu);
}