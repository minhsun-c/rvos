#ifndef __DEFS_H__
#define __DEFS_H__

#include <stddef.h>
#include "types.h"

/* printf.c */
int kprintf(const char *, ...);
void panic(char *s);

/* scanf.c */
int kscanf(const char *, ...);

/* memory.c */
void *memset(void *, int, size_t);
void *memcpy(void *, const void *, size_t);

/* kalloc.c */
void *kalloc(size_t);
void kfree(void *);

#endif  // __DEFS_H__