#include "defs.h"
#include "task.h"

#define DELAY 40000000L

static void myDelay(int Delay)
{
    for (unsigned long long i = 0; i < Delay; i++)
        ;
}

static void user_task0(void *p)
{
    kprintf("Task 0: Created!\n");

    while (1) {
        kprintf("Task 0: Running... \n");
        myDelay(DELAY);
        task_yield();
        kprintf("return Task 0 \n");
    }
}

static void user_task1(void *p)
{
    kprintf("Task 1: Created!\n");
    while (1) {
        kprintf("Task 1: Running... \n");
        myDelay(DELAY);
        task_yield();
        kprintf("return Task 1 \n");
    }
}

void loadTasks(void)
{
    task_t *task0, *task1;
    task0 = task_init("task0", user_task0, NULL, 1024, 10);
    task1 = task_init("task1", user_task1, NULL, 1024, 10);
    task_startup(task0);
    task_startup(task1);
}
