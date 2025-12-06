#include <stddef.h>
#include <string.h>

#include "defs.h"
#include "list.h"
#include "riscv.h"
#include "spinlock.h"
#include "task.h"
#include "types.h"

/* -------------------------------------------------------------------------- */
/*                            External Declarations                           */
/* -------------------------------------------------------------------------- */
extern void switch_to(struct context *);

/* -------------------------------------------------------------------------- */
/*                                 Globals                                    */
/* -------------------------------------------------------------------------- */
task_t task_list[256];       /* Static task pool */
task_t *task_running = NULL; /* Currently running task */
task_t task_ready;           /* Ready queue head (sentinel node) */
uint32_t task_idx;           /* Next TCB index for allocation */
spinlock_t task_lock;

/* -------------------------------------------------------------------------- */
/*                              Core Scheduler                                */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize the scheduler subsystem.
 *
 * - Clear mscratch to 0 to handle the first context switch safely.
 * - Initialize the ready queue list head.
 * - Reset the task allocation index.
 */
void sched_init(void)
{
    w_mscratch(0); /* Ensure first switch_to sees NULL */
    list_init((list_t *) &task_ready.list); /* Ready queue sentinel node */
    task_idx = 0;                           /* Reset TCB index */
    spinlock_init(&task_lock);
}

/**
 * @brief Pick the next task from the ready queue and perform a context switch.
 *
 * - Remove the next task from the ready list
 * - Reinsert current task back to ready list if applicable
 * - Update task_running pointer
 * - Switch to the new task context
 */
void schedule(void)
{
    acquire(&task_lock);

    /* Pick next task */
    task_t *nextTask = list_entry(task_ready.list.next, task_t, list);
    ctx_t *next = &nextTask->ctx;
    list_remove(&nextTask->list);

    /* Put the current task back into the ready queue */
    if (task_running != NULL) {
        task_t *currentTask = task_running;
        currentTask->state = TASK_READY;
        list_insert_before(&task_ready.list, &currentTask->list);
    }

    /* Switch to next task */
    task_running = nextTask;
    nextTask->state = TASK_RUNNING;

    release(&task_lock);
    switch_to(next);
}

/* -------------------------------------------------------------------------- */
/*                          Task Control Block (TCB) */
/* -------------------------------------------------------------------------- */

/**
 * @brief Allocate a new task control block from the static task pool.
 *
 * @return Pointer to a TCB from task_list[].
 */
static task_t *get_task(void)
{
    acquire(&task_lock);
    task_t *tcb = &task_list[task_idx];
    task_idx = (task_idx + 1) & 0xFF; /* Wrap around at 256 */
    release(&task_lock);
    return tcb;
}

/**
 * @brief Initialize a new task.
 *
 * - Allocate stack memory
 * - Initialize TCB fields
 * - Set entry point (ra) and stack pointer (sp)
 * - Insert task into initial state (not ready yet)
 */
task_t *task_init(const char *name,
                  taskFunc_t taskFunc,
                  void *parameter,
                  size_t stack_size,
                  uint16_t priority)
{
    void *stack_start = (void *) kalloc(stack_size);
    if (stack_start == NULL)
        return NULL;

    task_t *ptcb = get_task();

    memcpy(ptcb->name, name, sizeof(ptcb->name));
    ptcb->entry = taskFunc;
    ptcb->parameter = parameter;

    /* Initialize stack memory to zero */
    memset(stack_start, 0, stack_size);
    ptcb->stack_addr = stack_start;
    ptcb->stack_size = stack_size;

    /* Set initial context: ra = entry point, sp = top of stack */
    ptcb->ctx.ra = (uint32_t) taskFunc;
    ptcb->ctx.sp = (uint32_t) (stack_start + stack_size);

    ptcb->priority = priority;
    ptcb->state = TASK_INIT;

    /* Insert task as an isolated list node */
    list_init(&ptcb->list);

    return ptcb;
}

/* -------------------------------------------------------------------------- */
/*                                Task Control */
/* -------------------------------------------------------------------------- */

/**
 * @brief Start a task by transitioning from INIT → SUSPEND → READY.
 */
void task_startup(task_t *ptcb)
{
    ptcb->state = TASK_SUSPEND;
    task_resume(ptcb);
}

/**
 * @brief Resume a suspended task and move it into the ready queue.
 *
 * @return 0 on success, -1 if task state is not SUSPEND.
 */
uint32_t task_resume(task_t *ptcb)
{
    acquire(&task_lock);

    if (ptcb->state != TASK_SUSPEND) {
        release(&task_lock);
        return -1;
    }

    list_remove(&ptcb->list);
    list_insert_before(&task_ready.list, &ptcb->list);
    ptcb->state = TASK_READY;

    release(&task_lock);
    return 0;
}

/**
 * @brief Yield CPU from the current running task.
 *
 * - Move the current task to the end of the ready queue
 * - Trigger the scheduler
 */
uint32_t task_yield(void)
{
    acquire(&task_lock);
    task_t *ptcb = task_running;

    if (ptcb->state == TASK_READY) {
        /* Remove task from its current position */
        list_remove(&ptcb->list);
        /* Reinsert at the tail of the ready queue */
        list_insert_before(&task_ready.list, &ptcb->list);
    }
    release(&task_lock);

    schedule();
    return 0;
}
