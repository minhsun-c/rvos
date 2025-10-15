#ifndef __DEFS_H__
#define __DEFS_H__

/* uart */
void uart_init();
int uart_putc(char ch);
void uart_puts(char *s);
char uart_getc();

/* printf */
int kprintf(const char *s, ...);
void panic(char *s);

/* scanf */
int kscanf(const char *fmt, ...);

#endif  // __DEFS_H__