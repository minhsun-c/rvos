#include <stdarg.h>

extern int uart_putc(char);
extern void uart_puts(char *);
extern char uart_getc();

static void read_from_uart(char *buf, int maxlen)
{
    /* Finish the function */
}

static void parse_str(char *str)
{
    /* Finish the function */
}

static void parse_int(int *ptr)
{
    /* Finish the function */
}

static int _vscanf(const char *fmt, va_list ap)
{
    /* Finish the function */
}

int kscanf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int read_cnt = _vscanf(fmt, ap);

    va_end(ap);
    return read_cnt;
}
