#include <assert.h>
#include <math.h>

#include "../common/exec.h"
#include "../stack/log.h"
#include "cpu.h"

static CPU_ERRORS stack_dtor_unwrap (stack_t *stk, const char *const stk_name);

CPU_ERRORS run_binary (const void *binary, size_t binary_size)
{
    assert (binary != nullptr && "pointer can't be null");

    BIN_ERROR res = verify_binary (binary, binary_size, BINARY_VERSION, HEADER_VERSION);
    if (res != BIN_ERROR::OK)
    {
        log (log::ERR, "Invalid binary, error: %s", bin_strerror (res));
        return CPU_ERRORS::SYNTAX;
    }

    binary = (const char *)binary + sizeof (pre_header_t) + sizeof (header_t);
    size_t code_size  = ((const header_t *)binary)[-1].code_size;

    cpu_t *cpu = (cpu_t *) calloc (1, sizeof (cpu_t));
    if (cpu == nullptr) { return CPU_ERRORS::NOMEM; }
    cpu_init (cpu, binary, code_size);

    CPU_ERRORS exec_res = execute (cpu);

    free (cpu);
    return exec_res;
}

#ifdef CLEANUP_DEFINES
    #undef CLEANUP_DEFINES
#endif
#include "cpu_defines.h"

CPU_ERRORS execute (cpu_t *const cpu)
{
    assert (cpu != nullptr && "pointer can't be null");

    const opcode_t *instruct = nullptr;
    int op1 = 0, op2 = 0;
    int *op_ptr;

    for (cpu->in = 0; cpu->in < cpu->code_size;)
    {
        instruct = (const opcode_t *) (cpu->code + cpu->in);
        cpu->in += sizeof (opcode_t);

        switch (instruct->opcode) {

            #include "../common/opcodes.h"

            default:
                log (log::ERR, "Invalid opcode");
                return CPU_ERRORS::SYNTAX;
        }
    }

    log (log::WRN, "No halt opcode in binary");
    return CPU_ERRORS::OK;
}

#define CLEANUP_DEFINES
#include "cpu_defines.h"
#undef CLEANUP_DEFINES

int extract_arg (cpu_t *cpu, const opcode_t *const instruct)
{
    assert (cpu      != nullptr && "pointer can't be null");
    assert (instruct != nullptr && "pointer can't be null");
    
    int arg = 0;

    if (instruct->i)
    {
        arg += *(const int *) (cpu->code + cpu->in);
        cpu->in += sizeof (int);
    }
    if (instruct->r)
    {
        arg += cpu->regs[((const unsigned char *) cpu->code)[cpu->in]];
        cpu->in += sizeof (unsigned char);
    }
    if (instruct->m)
    {
        arg = cpu->ram[arg];
    }

    return arg;
}

int *extract_arg_pop (cpu_t *cpu, const opcode_t *const instruct)
{
    assert (cpu      != nullptr && "pointer can't be null");
    assert (instruct != nullptr && "pointer can't be null");
    
    int *arg_ptr = nullptr;
    opcode_t instruct_copy = *instruct;

    if (instruct -> m)
    {
        instruct_copy.m = false;
        arg_ptr = &cpu->ram[extract_arg (cpu, &instruct_copy)];
        instruct_copy.m = true;
    }
    else
    {
        if (instruct->i) return nullptr;
        
        arg_ptr = &cpu->regs[((const unsigned char *) cpu->code)[cpu->in]];
        cpu->in += sizeof (unsigned char);
    }

    return arg_ptr;
}

CPU_ERRORS cpu_init (cpu_t *cpu, const void* code, size_t code_size)
{
    assert (cpu  != nullptr && "pointer can't be null");
    assert (code != nullptr && "pointer can't be null");

    cpu->code      = (const char *) code;
    cpu->code_size = code_size;
    cpu->in        = 0;
    stack_ctor (&cpu->data_stk, sizeof (int));
    stack_ctor (&cpu->addr_stk, sizeof (int));

    return CPU_ERRORS::OK;
}

CPU_ERRORS cpu_free (cpu_t *cpu)
{
    assert (cpu != nullptr && "pointer can't be null");

    if (stack_dtor_unwrap (&cpu->addr_stk, "addr_stk") != CPU_ERRORS::OK)
    {
        return CPU_ERRORS::INTERNAL_ERROR;
    }
    
    if (stack_dtor_unwrap (&cpu->addr_stk, "data_stk") != CPU_ERRORS::OK)
    {
        return CPU_ERRORS::INTERNAL_ERROR;
    }

    return CPU_ERRORS::OK;
}

static CPU_ERRORS stack_dtor_unwrap (stack_t *stk, const char *const stk_name)
{
    err_flags dtor_res = stack_dtor (stk);
    if (dtor_res != res::OK)
    {
        log (log::ERR, "Failed to destruct internal stack (%s). Stack errors:", stk_name);
        stack_perror (dtor_res, get_log_stream(), "\t\t-->");
        return CPU_ERRORS::INTERNAL_ERROR;
    }

    return CPU_ERRORS::OK;
}