#include "platform.h"

# ============================================================
# Boot Entry (_start)
# ============================================================
# This code is executed by all harts (hardware threads) after reset.
# Only hart 0 continues to C code (start_kernel), while other harts
# enter a parked (idle) loop.
#
# Each hart has its own 1KB (1024 bytes) stack.
# ============================================================

    .equ    STACK_SIZE, 1024         # Size of each hart's stack (bytes)
    .global _start

    .text
_start:
    # ------------------------------------------------------------
    # 1. Identify the current hart
    # ------------------------------------------------------------
    csrr    t0, mhartid              # t0 = hartid
    mv      tp, t0                   # thread pointer (tp) = hartid

    # ------------------------------------------------------------
    # 2. Park non-zero harts (secondary CPUs)
    # ------------------------------------------------------------
    bnez    t0, park                 # If hartid != 0, jump to park loop

    # ------------------------------------------------------------
    # 3. Initialize stack for hart 0
    # ------------------------------------------------------------
    # Stack grows downward, so we set stack pointer ($sp) to the end of each hart’s stack area.

    slli    t0, t0, 10               # t0 = hartid * 1024
    la      sp, stacks + STACK_SIZE  # Load address of first stack’s end
    add     sp, sp, t0               # Offset $sp for this hart’s stack

    # ------------------------------------------------------------
    # 4. Jump to C code (kernel entry point)
    # ------------------------------------------------------------
    j       start_kernel             # Jump to C function 'start_kernel'


# ============================================================
# Park Loop for Non-Zero Harts
# ============================================================
# Secondary harts wait here indefinitely until the OS wakes them.
# 'wfi' (Wait-For-Interrupt) puts the hart into low-power sleep.
# ============================================================
park:
    wfi                             # Wait for interrupt (sleep)
    j       park                    # Loop forever


# ============================================================
# Stack Allocation Area
# ============================================================
# Reserve contiguous space for all hart stacks:
#   total size = STACK_SIZE * MAXNUM_CPU
# Each hart’s stack starts at: stacks + (hartid * STACK_SIZE)
# ============================================================
    .section .bss            
    .align 4                 
    .global stacks                  # Make it visible to C code
stacks:
    .skip STACK_SIZE * MAXNUM_CPU

    .end                            # End of assembly file
