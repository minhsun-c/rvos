#include "defs.h"
#include "task.h"
#include "types.h"

#define DELAY 40000000L
static void myDelay(int Delay)
{
    volatile unsigned long long i;
    for (i = 0; i < Delay; i++)
        ;
}

void user_task(void *p)
{
    while (1) {
        myDelay(DELAY);
        task_yield();
    }
}

void empty_test(void)
{
    task_t *task0, *task1;
    task0 = task_init("task0", user_task, NULL, 1024, 11);
    task1 = task_init("task1", user_task, NULL, 1024, 11);
    task_startup(task0);
    task_startup(task1);
}