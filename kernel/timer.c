#include "config.h"
#include "defs.h"
#include "platform.h"
#include "riscv.h"
#include "types.h"

uint32_t _tick = 0;

void timer_load(int interval)
{
    int id = r_mhartid();

    *(uint64_t *) CLINT_MTIMECMP(id) = *(uint64_t *) CLINT_MTIME + interval;
}

void timer_init()
{
    /*
     * On reset, mtime is cleared to zero, but the mtimecmp registers
     * are not reset. So we have to init the mtimecmp manually.
     */
    timer_load(SYSTEM_TICK);

    /* enable machine-mode timer interrupts. */
    w_mie(r_mie() | MIE_MTIE);

    /* enable machine-mode global interrupts. */
    w_mstatus(r_mstatus() | MSTATUS_MIE);
}

static void print_tick()
{
    uint32_t seconds = _tick % 60;
    uint32_t minutes = (_tick / 60) % 60;
    uint32_t hours = (_tick / 3600);
    kprintf("\r                   \r");
    if (hours < 10)
        kprintf("0");
    kprintf("%d:", hours);
    if (minutes < 10)
        kprintf("0");
    kprintf("%d:", minutes);
    if (seconds < 10)
        kprintf("0");
    kprintf("%d", seconds);
}

void timer_handler()
{
    _tick++;
    print_tick();
    timer_load(SYSTEM_TICK);
}