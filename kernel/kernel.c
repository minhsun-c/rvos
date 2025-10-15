extern int kprintf(const char *, ...);
extern void uart_init(void);
extern void page_init(void);

void start_kernel(void)
{
    uart_init();
    page_init();
    kprintf("Hello, RVOS!\n");

    while (1)
        ;
}
