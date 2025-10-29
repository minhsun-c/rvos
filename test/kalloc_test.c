#include <stddef.h>
#include "defs.h"
#include "types.h"

#define ALIGNMENT 8

void kalloc_test(void)
{
    kprintf("=== kalloc_test ===\n");

    void *p1 = kalloc(31);
    void *p2 = kalloc(64);
    void *p3 = kalloc(128);

    kprintf("p1 = %p\n", p1);
    kprintf("p2 = %p\n", p2);
    kprintf("p3 = %p\n", p3);

    // Check alignment
    if (((unsigned int) p1 & (ALIGNMENT - 1)) != 0)
        kprintf("p1 not aligned!\n");
    if (((unsigned int) p2 & (ALIGNMENT - 1)) != 0)
        kprintf("p2 not aligned!\n");
    if (((unsigned int) p3 & (ALIGNMENT - 1)) != 0)
        kprintf("p3 not aligned!\n");

    kfree(p2);  // free middle block

    void *p4 = kalloc(256);
    kprintf("p4 = %p\n", p4);

    void *p5 = kalloc(48);  // should reuse p2's block
    kprintf("p5 = %p\n", p5);

    kfree(p1);
    kfree(p3);
    kfree(p4);
    kfree(p5);
}