#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

typedef unsigned int uint32_t;

struct spinlock {
    volatile uint32_t locked;  // 判斷是否有被佔有(佔有:1，未佔有:0)
};

#endif  // __SPINLOCK_H__