extern int kprintf(const char *, ...);
extern void uart_init(void);
extern void kmem_init(void);
extern void sched_init(void);
extern void schedule(void);

/* Adding Tests */
extern void loadTasks(void);

void start_kernel(void)
{
    uart_init();
    kmem_init();
    sched_init();
    kprintf("Hello, RVOS!\n");

    loadTasks();

    schedule();
    while (1)
        ;
}
