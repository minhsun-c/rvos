#include "types.h"
extern int kprintf(const char *, ...);
extern void uart_init(void);
extern void kmem_init(void);
extern void sched_init(void);
extern void trap_init(void);
extern void timer_init(void);
extern void vga_init(void);
extern void schedule(void);

void empty_test(void);
void vga_test(void);

void start_kernel(void)
{
    uart_init();
    vga_init();
    kmem_init();
    trap_init();
    timer_init();
    sched_init();
    kprintf("Hello, RVOS!\n\r");

#ifdef VGA_NYANCAT_TEST
    kprintf("NYANCAT\n");
    vga_test();
#else
    kprintf("NORMAL\n");
    empty_test();
#endif

    schedule();
    while (1)
        ;
}
