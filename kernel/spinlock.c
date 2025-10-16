#include "spinlock.h"
#include "riscv.h"

void spinlock_init(spinlock_t *lk)
{
    lk->locked = 0;
}

int acquire(spinlock_t *lk)
{
    while (__sync_lock_test_and_set(&lk->locked, 1) != 0)
        ;
    return 0;
}

int release(spinlock_t *lk)
{
    __sync_lock_release(&lk->locked);
    return 0;
}
