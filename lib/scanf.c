#include <stdarg.h>
#include "os.h"

static void read_from_uart(char *buf, int maxlen)
{
    int pos = 0;
    for (;;) {
        char ch = uart_getc();

        // Handle backspace (optional)
        if (ch == '\b' || ch == 0x7f) {
            if (pos > 0) {
                pos--;
                uart_puts("\b \b");
            }
            continue;
        }

        // Stop at space or newline
        if (ch == ' ' || ch == '\n' || ch == '\r') {
            uart_putc('\n');
            break;
        }

        // Store char
        if (pos < maxlen - 1) {
            buf[pos++] = ch;
            uart_putc(ch);  // echo back
        }
    }
    buf[pos] = '\0';
}

static void parse_str(char *str)
{
    read_from_uart(str, 128);
}

static void parse_int(int *ptr)
{
    char buf[128];
    read_from_uart(buf, 128);
    int sign = 1;
    int val = 0;
    int idx = 0;
    if (buf[idx] == '-') {
        sign = -1;
        idx++;
    }
    while (buf[idx] >= '0' && buf[idx] <= '9') {
        val = val * 10 + (buf[idx] - '0');
        idx++;
    }
    *ptr = val * sign;
}

static int _vscanf(const char *fmt, va_list ap)
{
    int read_cnt = 0;
    char buf[64];

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            if (*fmt == 0) {
                break;
            } else if (*fmt == 's') {
                char *s = va_arg(ap, char *);
                parse_str(s);
                read_cnt++;
            } else if (*fmt == 'd') {
                int *p = va_arg(ap, int *);
                parse_int(p);
                read_cnt++;
            }
        }
        fmt++;
    }
    return read_cnt;
}

int kscanf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);


    int read_cnt = _vscanf(fmt, ap);

    va_end(ap);
    return read_cnt;
}
