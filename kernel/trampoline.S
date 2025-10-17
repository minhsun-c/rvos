#include "ctx.inc"
.text

# -----------------------------------------------------------------------------
#  void trap_vector(void);
#  ----------------------
#  Machine trap entry point.
#
#  Flow:
#    1. Swap t6 with mscratch to obtain current context pointer.
#    2. Save all GP registers to the current context.
#    3. Save the actual t6 value (since it’s used as the base).
#    4. Call C trap handler: trap_handler(mepc, mcause).
#    5. Update mepc with handler’s return value (a0).
#    6. Restore all GP registers from the current context.
#    7. mret back to interrupted code.
#
#  Note:
#    - mscratch holds a pointer to the *current* task’s context.
#    - t6 (x31) is used as the base pointer for reg_save/reg_restore.
#    - t6 must be saved/restored separately in the context structure.
# -----------------------------------------------------------------------------
.globl trap_vector
.align 4
trap_vector:
    # Swap t6 with mscratch to get current context pointer
    csrrw   t6, mscratch, t6           # t6 = old mscratch (ctx ptr), mscratch = t6

    # Save all GP registers (except t6 which is base)
    reg_save t6                        # save GP regs to *t6

    # Save the actual t6 value into the context
    mv      t5, t6                     # t5 = context pointer (base)
    csrr    t6, mscratch               # t6 = original t6 value (swapped earlier)
    sw      t6, CONTEXT_t6(t5)         # store t6 in context
    csrw    mscratch, t5               # restore mscratch to hold ctx pointer

    # -------------------------------------------------------------------------
    # Call the C trap handler: 
    #     uint32_t trap_handler(uint32_t epc, uint32_t cause)
    # -------------------------------------------------------------------------
    csrr    a0, mepc                   # a0  = mepc
    csrr    a1, mcause                 # a1  = mcause
    call    trap_handler               # a0' = return value
    csrw    mepc, a0                   # mepc = a0'

    # Restore all GP registers (including t6)
    csrr    t6, mscratch               # reload context pointer
    reg_restore t6

    mret
