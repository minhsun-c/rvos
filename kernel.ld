/* 
 * Linker script for a simple RISC-V bare-metal OS.
 * 
 * This script describes the memory layout and how to place different
 * sections (text, rodata, data, bss) in  RAM. It also provides symbols
 * for use in C/assembly (e.g., BSS_START) to help the kernel initialize
 * memory and manage heap.
 */

/* 
 * OUTPUT_ARCH sets the target machine architecture for the output file.
 * For both RV32 and RV64, the architecture string is "riscv".
 */
OUTPUT_ARCH("riscv")

/*
 * ENTRY specifies the entry point symbol.
 * The prog RAM will start execution at the address of _start,
 * which should be defined in start.S.
 */
ENTRY(_start)

/*
 * Define memory regions.
 * - ORIGIN: start address of the memory
 * - LENGTH: total size of the memory
 * The 'wxa!ri' flags mean:
 *   w = writable
 *   x = executable
 *   a = allocatable
 *   !r = not readable by debugger
 *   i = initialized
 */
MEMORY
{
    RAM (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 128M
}

/*
 * SECTIONS defines how input sections from object files are mapped
 * to output sections in the final ELF file.
 */
SECTIONS
{
    /* 
     * .text section: executable code
     * We also provide TEXT_START and TEXT_END symbols for debugging
     * and possible protection setups.
     */
    .text : {
        PROVIDE(TEXT_START = .);
        *(.text .text.*)
        PROVIDE(TEXT_END = .);
    } > RAM

    /*
     * .rodata section: read-only data (e.g., const strings, lookup tables)
     * Placed right after .text in memory.
     */
    .rodata : {
        PROVIDE(RODATA_START = .);
        *(.rodata .rodata.*)
        PROVIDE(RODATA_END = .);
    } > RAM

    /*
     * .data section: initialized global/static variables
     * We align to 4096 to keep page alignment for easier MMU or
     * memory management integration in the future.
     * sdata is merged into data for simplicity.
     */
    .data : {
        . = ALIGN(4096);
        PROVIDE(DATA_START = .);
        *(.sdata .sdata.*)
        *(.data .data.*)
        PROVIDE(DATA_END = .);
    } > RAM

    /*
     * .bss section: zero-initialized global/static variables
     * Also includes COMMON symbols (uninitialized globals).
     * The kernel will typically zero this region during early boot.
     */
    .bss : {
        PROVIDE(BSS_START = .);
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        PROVIDE(BSS_END = .);
    } > RAM

    /*
     * Provide useful linker symbols for runtime memory management.
     * These are not sections but symbols that can be used in C code.
     */
    PROVIDE(MEMORY_START = ORIGIN(RAM));
    PROVIDE(MEMORY_END = ORIGIN(RAM) + LENGTH(RAM));

    /*
     * Heap starts right after .bss ends.
     * The OS can use HEAP_START and HEAP_SIZE to manage dynamic memory.
     */
    PROVIDE(HEAP_START = BSS_END);
    PROVIDE(HEAP_SIZE = MEMORY_END - HEAP_START);
}
