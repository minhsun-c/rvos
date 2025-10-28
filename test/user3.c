#include "defs.h"
#include "task.h"
#include "types.h"

#define DELAY 40000000L
static void myDelay(int Delay)
{
    for (unsigned long long i = 0; i < Delay; i++)
        ;
}

void my_trap()
{
    int a = *(int *) 0x00000000;
    a = 100;
    kprintf("Return back from trap! %d\n", a);
}

void user_task0(void *p)
{
    kprintf("Task 0: Created!\n");

    while (1) {
        kprintf("Task 0: Running... \n");
        myDelay(DELAY);
        my_trap();
        task_yield();
        kprintf("return Task 0 \n");
    }
}

void user_task1(void *p)
{
    kprintf("Task 1: Created!\n");
    while (1) {
        kprintf("Task 1: Running... \n");
        myDelay(DELAY);
        task_yield();
        kprintf("return Task 1 \n");
    }
}

void user_task2(void *p)
{
    kprintf("Task 2: Created!\n");
    while (1) {
        kprintf("Task 2: Running... \n");
        myDelay(DELAY);
        task_yield();
        kprintf("return Task 2 \n");
    }
}

void trap_test(void)
{
    task_t *task0, *task1, *task2;
    task0 = task_init("task0", user_task0, NULL, 1024, 11);
    task1 = task_init("task1", user_task1, NULL, 1024, 11);
    task2 = task_init("task2", user_task2, NULL, 1024, 11);
    task_startup(task0);
    task_startup(task1);
    task_startup(task2);
}