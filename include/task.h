#ifndef __TASK_H__
#define __TASK_H__

#include "list.h"
#include "types.h"

/* -------------------------------------------------------------------------- */
/*                              Task State Enum                               */
/* -------------------------------------------------------------------------- */

/**
 * @brief Task states used in the scheduler.
 */
enum task_state {
    TASK_INIT = 0, /**< Task is created but not started */
    TASK_READY,    /**< Task is ready to run */
    TASK_SUSPEND,  /**< Task is suspended (waiting) */
    TASK_RUNNING   /**< Task is currently running */
};

/* -------------------------------------------------------------------------- */
/*                              CPU Context Frame                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief CPU register context saved during context switch.
 *
 * This structure must match the order in which registers are
 * saved/restored in `switch_to` (swtch.S).
 *
 * ra   - return address
 * sp   - stack pointer
 * gp   - global pointer
 * tp   - thread pointer
 * t0~t6, s0~s11, a0~a7 - caller and callee saved registers
 * pc   - program counter
 */
struct context {
    uint32_t ra;
    uint32_t sp;
    uint32_t gp;
    uint32_t tp;

    uint32_t t0;
    uint32_t t1;
    uint32_t t2;

    uint32_t s0;
    uint32_t s1;

    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;

    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;

    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;

    /* Saved program counter */
    uint32_t pc;
};

/* -------------------------------------------------------------------------- */
/*                                Task Control                                */
/* -------------------------------------------------------------------------- */

/**
 * @brief Task Control Block (TCB).
 *
 * Each task in the system is represented by a TCB. It holds
 * scheduling information, stack management, CPU context, and
 * linkage to the ready/suspend lists.
 */
struct task {
    list_t node; /**< List node for ready/suspend queue linkage */

    char name[10];   /**< Human-readable name (not null-terminated if full) */
    uint32_t taskID; /**< Task ID or index in task pool */

    taskFunc_t entry; /**< Task entry function */
    void *parameter;  /**< Entry function parameter */

    void *stack_addr;  /**< Pointer to allocated stack base */
    size_t stack_size; /**< Stack size in bytes */
    void *sp;          /**< Current stack pointer */

    ctx_t ctx; /**< Saved CPU context */

    state_t state;    /**< Current task state */
    uint8_t priority; /**< Task priority (lower value = higher priority) */
};

#endif  // __TASK_H__
