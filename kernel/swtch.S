# -----------------------------------------------------------------------------
#  Macro: reg_save base
#  ---------------------
#  Save all General-Purpose (GP) registers into the context structure pointed
#  to by \base.
#
#  struct context *base = &ctx_task;
#  base->ra = ra;
#  base->sp = sp;
#  ...
#
#  Note:
#    - t6 is used as the base pointer for saving.
#    - t6 itself is saved separately after reg_save, because it’s being used
#      as the base register during the save process.
# -----------------------------------------------------------------------------
.macro reg_save base
    sw ra,  0(\base)
    sw sp,  4(\base)
    sw gp,  8(\base)
    sw tp, 12(\base)
    sw t0, 16(\base)
    sw t1, 20(\base)
    sw t2, 24(\base)
    sw s0, 28(\base)
    sw s1, 32(\base)
    sw a0, 36(\base)
    sw a1, 40(\base)
    sw a2, 44(\base)
    sw a3, 48(\base)
    sw a4, 52(\base)
    sw a5, 56(\base)
    sw a6, 60(\base)
    sw a7, 64(\base)
    sw s2, 68(\base)
    sw s3, 72(\base)
    sw s4, 76(\base)
    sw s5, 80(\base)
    sw s6, 84(\base)
    sw s7, 88(\base)
    sw s8, 92(\base)
    sw s9, 96(\base)
    sw s10, 100(\base)
    sw s11, 104(\base)
    sw t3, 108(\base)
    sw t4, 112(\base)
    sw t5, 116(\base)
    # t6 will be saved separately outside this macro
.endm


# -----------------------------------------------------------------------------
#  Macro: reg_restore base
#  ------------------------
#  Restore all General-Purpose (GP) registers from the context structure
#  pointed to by \base.
#
#  struct context *base = &ctx_task;
#  ra = base->ra;
#  sp = base->sp;
#  ...
#
#  Note:
#    - t6 is restored at the end of this macro (unlike reg_save).
# -----------------------------------------------------------------------------
.macro reg_restore base
    lw ra,  0(\base)
    lw sp,  4(\base)
    lw gp,  8(\base)
    lw tp, 12(\base)
    lw t0, 16(\base)
    lw t1, 20(\base)
    lw t2, 24(\base)
    lw s0, 28(\base)
    lw s1, 32(\base)
    lw a0, 36(\base)
    lw a1, 40(\base)
    lw a2, 44(\base)
    lw a3, 48(\base)
    lw a4, 52(\base)
    lw a5, 56(\base)
    lw a6, 60(\base)
    lw a7, 64(\base)
    lw s2, 68(\base)
    lw s3, 72(\base)
    lw s4, 76(\base)
    lw s5, 80(\base)
    lw s6, 84(\base)
    lw s7, 88(\base)
    lw s8, 92(\base)
    lw s9, 96(\base)
    lw s10, 100(\base)
    lw s11, 104(\base)
    lw t3, 108(\base)
    lw t4, 112(\base)
    lw t5, 116(\base)
    lw t6, 120(\base)
.endm


# -----------------------------------------------------------------------------
#  Context switching notes
#  -----------------------
#  - mscratch holds a pointer to the context of the *previous* task.
#  - t6 is used as the base for reg_save/reg_restore because:
#      * It is x31, the last GP register.
#      * Using it as a base avoids overwriting it during restore.
#  - t6 itself must be saved/restored separately (see switch_to).
# -----------------------------------------------------------------------------


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
    sw          t6, 120(t5)            # save t6 in previous context

1:
    # Update mscratch to point to the next context
    csrw        mscratch, a0

    # Restore context of the next task
    mv          t6, a0
    reg_restore t6

    # Return to next task (restored ra)
    ret

.end
