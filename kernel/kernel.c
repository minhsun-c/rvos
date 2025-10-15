extern int kprintf(const char *, ...);
extern int kscanf(const char *, ...);
extern void kecho(void);
extern void uart_init(void);

void test_io() 
{
    int x, y;
    kprintf("Input x, y: ");
    kscanf("%d %d", &x, &y);
    kprintf("x * y = %d\n", x * y);
}

void start_kernel(void)
{
    uart_init();
    kprintf("Hello, RVOS!\n");

    test_io();
    kecho();

    while (1)
        ;
}
