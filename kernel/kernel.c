extern int kprintf(const char *, ...);
extern void uart_init(void);
extern void page_init(void);
extern void sched_init(void);
extern void loadTasks(void);
extern void schedule(void);

void start_kernel(void)
{
    uart_init();
    page_init();
    kprintf("Hello, RVOS!\n");

    sched_init();
    loadTasks();
    schedule();
}
