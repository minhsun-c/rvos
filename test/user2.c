#include "defs.h"
#include "spinlock.h"
#include "task.h"
#include "types.h"

static volatile int shared = 0;
static spinlock_t lock;

void worker(void *p)
{
    int times = 1000;
    for (int i = 0; i < times; i++) {
        acquire(&lock);
        int tmp = shared;
        for (volatile int j = 0; j < 100; j++)
            ;
        shared = tmp + 1;
        if (shared <= 300)
            kprintf("[spinlock_test] shared = %d\n", shared);
        release(&lock);
        task_yield();
    }
}

void spinlock_test(void)
{
    shared = 0;
    spinlock_init(&lock);
    for (int i = 0; i < 3; i++)
        task_startup(task_init("w", worker, NULL, 1024, 11));
    // after some time, shared should equal 3 * 1000
}
