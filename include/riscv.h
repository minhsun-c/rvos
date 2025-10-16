#ifndef __RISCV_H__
#define __RISCV_H__

typedef unsigned char uint8_t;

#include "types.h"


/* Machine Status Register, mstatus */
#define MSTATUS_MIE (1 << 3)
#define MSTATUS_SIE (1 << 1)
#define MSTATUS_UIE (1 << 0)

static inline uint8_t r_mstatus()
{
    uint8_t x;
    asm volatile("csrr %0, mstatus" : "=r"(x));
    return x;
}

static inline void w_mstatus(uint8_t x)
{
    asm volatile("csrw mstatus, %0" : : "r"(x));
}

static inline void w_mscratch(uint8_t x)
{
    asm volatile("csrw mscratch, %0" : : "r"(x));
}

#endif  // __RISCV_H__
