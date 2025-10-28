#include "defs.h"
#include "riscv.h"
#include "types.h"

extern char trap_vector[];

void trap_init()
{
    /*
     * set the trap-vector base-address for machine-mode
     */
    w_mtvec((uint32_t) trap_vector);
}

uint32_t trap_handler(uint32_t epc, uint32_t cause)
{
    uint32_t return_pc = epc;
    uint32_t cause_code = cause & 0xfff;

    if (cause & 0x80000000) {
        /* Asynchronous trap - interrupt */
        switch (cause_code) {
        case 3:
            kprintf("[trap] software interrupt\n");
            break;
        case 7:
            kprintf("[trap] timer interrupt\n");
            break;
        case 11:
            kprintf("[trap] external interrupt\n");
            break;
        default:
            kprintf("[trap] unknown interrupt (code %lu)\n", cause_code);
            break;
        }
    } else {
        /* Synchronous trap - exception */
        kprintf("[trap] Sync exception, code = %lu\n", cause_code);
        return_pc += 4;  // skip faulting instruction (no C extension)
    }

    return return_pc;
}
