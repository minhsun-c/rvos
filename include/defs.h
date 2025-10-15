#ifndef __DEFS_H__
#define __DEFS_H__

/* printf */
int kprintf(const char *s, ...);
void panic(char *s);

/* scanf */
int kscanf(const char *fmt, ...);

#endif  // __DEFS_H__