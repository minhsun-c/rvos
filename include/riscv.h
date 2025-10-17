#ifndef __RISCV_H__
#define __RISCV_H__

#include "types.h"

/* Machine Status Register, mstatus */
#define MSTATUS_MIE (1 << 3)
#define MSTATUS_SIE (1 << 1)
#define MSTATUS_UIE (1 << 0)

static inline uint32_t r_mstatus()
{
    uint32_t x;
    asm volatile("csrr %0, mstatus" : "=r"(x));
    return x;
}

static inline void w_mstatus(uint32_t x)
{
    asm volatile("csrw mstatus, %0" : : "r"(x));
}

static inline void w_mscratch(uint32_t x)
{
    asm volatile("csrw mscratch, %0" : : "r"(x));
}

/* Machine-mode interrupt vector */
static inline void w_mtvec(uint32_t x)
{
	asm volatile("csrw mtvec, %0" : : "r" (x));
}

#endif  // __RISCV_H__
