extern int kprintf(const char *, ...);
extern void uart_init(void);
extern void page_init(void);
extern void sched_init(void);
extern void trap_init(void);
extern void schedule(void);

void start_kernel(void)
{
    uart_init();
    page_init();
    trap_init();
    sched_init();
    kprintf("Hello, RVOS!\n");

    schedule();
}
