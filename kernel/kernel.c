extern int kprintf(const char *, ...);
extern void uart_init(void);
extern void kmem_init(void);
extern void kalloc_test(void);

void start_kernel(void)
{
    uart_init();
    kmem_init();
    kprintf("Hello, RVOS!\n");

    kalloc_test();
    while (1)
        ;
}
