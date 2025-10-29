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

/* task.c */
void schedule(void);
task_t *task_init(const char *, taskFunc_t, void *, size_t, uint16_t);
void task_startup(task_t *);
uint32_t task_resume(task_t *);
uint32_t task_yield(void);

/* spinlock.c */
void spinlock_init(spinlock_t *);
int acquire(spinlock_t *);
int release(spinlock_t *);

/* trap.c */
uint32_t trap_handler(uint32_t, uint32_t);

#endif  // __DEFS_H__