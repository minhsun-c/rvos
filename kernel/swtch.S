#include "ctx.inc"

.text
.globl switch_to
.align 4

# -----------------------------------------------------------------------------
# void switch_to(struct context *prev, struct context *next);
# -----------------------------------------------------------------------------
# a0 : Pointer to the PREVIOUS context (save state here)
# a1 : Pointer to the NEXT context (restore state from here)
# -----------------------------------------------------------------------------
switch_to:
    # Save the current context to 'prev' (a0)
    reg_save a0               # Use ctx.inc macro (saves ra, sp, gp, tp, t0-t5, s0-s11, a0-a7)
    sw t6, CONTEXT_t6(a0)     # Manually save t6 (since reg_save excludes it)

    # Update mscratch
    # Point mscratch to the "next" task's context.
    # This ensures that if an interrupt occurs (Trap Handler), 
    # it can find the correct location to save the context.
    csrw mscratch, a1

    # Restore the 'next' context from (a1)
    # Move a1 (next pointer) to t6 to use it as the base register for loading.
    mv t6, a1
    
    # Use ctx.inc macro to restore all registers.
    # Note: The reg_restore macro ends with 'lw t6, offset(base)'.
    # Therefore, after this executes, t6 will contain the restored value,
    # not the context pointer.
    reg_restore t6            

    ret