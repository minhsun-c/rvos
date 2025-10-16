#ifndef __RISCV_H__
#define __RISCV_H__

typedef unsigned char uint8_t;

static inline void w_mscratch(uint8_t x)
{
    asm volatile("csrw mscratch, %0" : : "r"(x));
}

#endif  // __RISCV_H__
