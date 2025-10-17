/* ctx.inc — context layout + macros (no fancy GAS features) */
#ifndef CTX_INC
#define CTX_INC 1

/* Offsets (bytes) inside struct context */
.set CONTEXT_ra,   0
.set CONTEXT_sp,   4
.set CONTEXT_gp,   8
.set CONTEXT_tp,  12
.set CONTEXT_t0,  16
.set CONTEXT_t1,  20
.set CONTEXT_t2,  24
.set CONTEXT_s0,  28
.set CONTEXT_s1,  32
.set CONTEXT_a0,  36
.set CONTEXT_a1,  40
.set CONTEXT_a2,  44
.set CONTEXT_a3,  48
.set CONTEXT_a4,  52
.set CONTEXT_a5,  56
.set CONTEXT_a6,  60
.set CONTEXT_a7,  64
.set CONTEXT_s2,  68
.set CONTEXT_s3,  72
.set CONTEXT_s4,  76
.set CONTEXT_s5,  80
.set CONTEXT_s6,  84
.set CONTEXT_s7,  88
.set CONTEXT_s8,  92
.set CONTEXT_s9,  96
.set CONTEXT_s10, 100
.set CONTEXT_s11, 104
.set CONTEXT_t3,  108
.set CONTEXT_t4,  112
.set CONTEXT_t5,  116
.set CONTEXT_t6,  120   /* saved separately in your flow */

/* Save all GP regs except t6 (you save it separately) */
.macro reg_save base
    sw ra,  CONTEXT_ra(\base)
    sw sp,  CONTEXT_sp(\base)
    sw gp,  CONTEXT_gp(\base)
    sw tp,  CONTEXT_tp(\base)
    sw t0,  CONTEXT_t0(\base)
    sw t1,  CONTEXT_t1(\base)
    sw t2,  CONTEXT_t2(\base)
    sw s0,  CONTEXT_s0(\base)
    sw s1,  CONTEXT_s1(\base)
    sw a0,  CONTEXT_a0(\base)
    sw a1,  CONTEXT_a1(\base)
    sw a2,  CONTEXT_a2(\base)
    sw a3,  CONTEXT_a3(\base)
    sw a4,  CONTEXT_a4(\base)
    sw a5,  CONTEXT_a5(\base)
    sw a6,  CONTEXT_a6(\base)
    sw a7,  CONTEXT_a7(\base)
    sw s2,  CONTEXT_s2(\base)
    sw s3,  CONTEXT_s3(\base)
    sw s4,  CONTEXT_s4(\base)
    sw s5,  CONTEXT_s5(\base)
    sw s6,  CONTEXT_s6(\base)
    sw s7,  CONTEXT_s7(\base)
    sw s8,  CONTEXT_s8(\base)
    sw s9,  CONTEXT_s9(\base)
    sw s10, CONTEXT_s10(\base)
    sw s11, CONTEXT_s11(\base)
    sw t3,  CONTEXT_t3(\base)
    sw t4,  CONTEXT_t4(\base)
    sw t5,  CONTEXT_t5(\base)
.endm

/* Restore all GP regs including t6 at the end */
.macro reg_restore base
    lw ra,  CONTEXT_ra(\base)
    lw sp,  CONTEXT_sp(\base)
    lw gp,  CONTEXT_gp(\base)
    lw tp,  CONTEXT_tp(\base)
    lw t0,  CONTEXT_t0(\base)
    lw t1,  CONTEXT_t1(\base)
    lw t2,  CONTEXT_t2(\base)
    lw s0,  CONTEXT_s0(\base)
    lw s1,  CONTEXT_s1(\base)
    lw a0,  CONTEXT_a0(\base)
    lw a1,  CONTEXT_a1(\base)
    lw a2,  CONTEXT_a2(\base)
    lw a3,  CONTEXT_a3(\base)
    lw a4,  CONTEXT_a4(\base)
    lw a5,  CONTEXT_a5(\base)
    lw a6,  CONTEXT_a6(\base)
    lw a7,  CONTEXT_a7(\base)
    lw s2,  CONTEXT_s2(\base)
    lw s3,  CONTEXT_s3(\base)
    lw s4,  CONTEXT_s4(\base)
    lw s5,  CONTEXT_s5(\base)
    lw s6,  CONTEXT_s6(\base)
    lw s7,  CONTEXT_s7(\base)
    lw s8,  CONTEXT_s8(\base)
    lw s9,  CONTEXT_s9(\base)
    lw s10, CONTEXT_s10(\base)
    lw s11, CONTEXT_s11(\base)
    lw t3,  CONTEXT_t3(\base)
    lw t4,  CONTEXT_t4(\base)
    lw t5,  CONTEXT_t5(\base)
    lw t6,  CONTEXT_t6(\base)
.endm

#endif /* CTX_INC */
