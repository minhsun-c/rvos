# RVOS: A Lightweight RISC-V Operating System

**rvos** is a bare-metal operating system kernel written in **C** and **Assembly** for the **RISC-V** architecture.  
It serves as an educational project for exploring low-level system programming, memory management, and kernel architecture.

---

## Features

### Architecture
- Target: **RISC-V**
- Tested on **QEMU virt** machine

### Boot Process
- Assembly startup file (`start.S`)
- Initializes stack pointer
- Handles multi-core (Hart) parking

### Memory Management
- **Custom kalloc heap allocator**
- **Stack Safety**
  - Kernel stack placed in `.bss`
  - Ensures writable memory and known bounds for GC
- **Direct Memory Mapping**
  - Running in **Machine Mode**
  - Identity mapping: PA = VA

---

## Project Structure

| Directory   | Description |
|-------------|-------------|
| `kernel/`   | Core kernel code (allocator, scheduler, main entry). |
| `startup/`  | Assembly boot code (`start.S`), hardware init. |
| `include/`  | Header files & hardware definitions. |
| `lib/`      | Utility functions (string ops, misc libs). |
| `test/`     | Kernel-level test code. |
| `kernel.ld` | Linker script defining memory layout. |

---

## Building and Running

### Prerequisites
- Toolchain: `riscv64-unknown-elf-gcc` (or riscv32 equivalent)
- Emulator: QEMU (`qemu-system-riscv64` / `qemu-system-riscv32`)
- Build Tool: GNU Make

### Build and Run in QEMU
```
make run
```

### Debugging

```
make debug
```

---

## Memory Layout

The memory layout begins at **0x80000000**, defined in `kernel.ld`:

- `.text` – Kernel code (read-only, executable)  
- `.data` – Initialized global variables  
- `.bss` – Uninitialized globals & kernel stack  
- **Heap** – Starts after `.bss`, grows upward to `MEMORY_END`

---

##  License
This project is open source.
