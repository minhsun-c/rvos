#include "ctx.inc"
.text

# -----------------------------------------------------------------------------
#  void switch_to(struct context *next);
#  ------------------------------------
#  a0 : pointer to the context of the next task
#
#  This function performs a full context switch:
#    1. Swap t6 with mscratch to get previous context pointer.
#    2. Save all registers of the previous task (if it exists).
#    3. Save the actual value of t6 (because it's used as base).
#    4. Update mscratch to point to the next context.
#    5. Restore all registers from the next task’s context.
#    6. Return to the next task (ra is restored).
# -----------------------------------------------------------------------------
.globl switch_to
.align 4
switch_to:
    csrrw       t6, mscratch, t6       # Swap t6 and mscratch
    beqz        t6, 1f                 # Previous task may be NULL
    reg_save    t6                     # Save context of previous task

    # Save the actual t6 register (since it was used as base)
    mv          t5, t6                 # t5 = pointer to previous context
    csrr        t6, mscratch           # restore t6 from mscratch
    sw          t6, CONTEXT_t6(t5)     # save t6 in previous context

1:
    # Update mscratch to point to the next context
    csrw        mscratch, a0

    # Restore context of the next task
    mv          t6, a0
    reg_restore t6

    # Return to next task (restored ra)
    ret

.end
