VGA_ENABLE ?= 0

CROSS_COMPILE = riscv64-unknown-elf-
CFLAGS        = -nostdlib -fno-builtin -march=rv32imazicsr -mabi=ilp32 -g -Wall
CFLAGS       += -Wl,--no-warn-rwx-segments

ifeq ($(VGA_ENABLE), 1)
    CFLAGS += -DVGA_NYANCAT_TEST
endif

QEMU    = qemu-system-riscv32
Q_BASE_FLAGS = -nographic -smp 1 -machine virt -bios none
Q_VGA_FLAGS  = -smp 1 -machine virt -bios none -m 256M -monitor stdio
Q_VGA_FLAGS += -device virtio-vga -display cocoa,zoom-to-fit=on

GDB    ?= $(shell command -v $(CROSS_COMPILE)gdb || command -v gdb-multiarch || command -v gdb)
CC      = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump

LINK_SCRIPT = kernel.ld

SRCS_ASM += $(wildcard ./startup/*.S)
SRCS_ASM += $(wildcard ./kernel/*.S)

SRCS_C   += $(wildcard ./lib/*.c)
SRCS_C   += $(wildcard ./kernel/*.c)
SRCS_C   += $(wildcard ./test/*.c)

INC += -I./include

OBJS  = $(patsubst %.S, objs/%.o, $(notdir ${SRCS_ASM}))
OBJS += $(patsubst %.c, objs/%.o, $(notdir ${SRCS_C}))

.DEFAULT_GOAL := all
all: os.elf

# start.o must be the first in dependency!
os.elf: ${OBJS}
	${CC} ${CFLAGS} -T $(LINK_SCRIPT) -o os.elf $^
	${OBJCOPY} -O binary os.elf os.bin

objs/%.o: ${SRCS_ASM} ${SRCS_C}
	${CC} ${CFLAGS} ${INC} -c $(filter %/$(*F).c,$^) $(filter %/$(*F).S,$^) -o $@

run: all
	@${QEMU} -M ? | grep virt >/dev/null || exit
	@${QEMU} ${Q_BASE_FLAGS} -kernel os.elf

run-vga: 
	@$(MAKE) all VGA_ENABLE=1
	@${QEMU} -M ? | grep virt >/dev/null || exit
	@${QEMU} ${Q_VGA_FLAGS} -kernel os.elf

.PHONY: debug
debug: all
	@${QEMU} ${Q_BASE_FLAGS} -kernel os.elf -s -S &
	@${GDB} os.elf -q -x gdbinit

debug_vscode: all
	@${QEMU} ${Q_BASE_FLAGS} -kernel os.elf -s -S &

.PHONY: code
code: all
	@${OBJDUMP} -S os.elf | less

.PHONY: clean
clean:
	rm -rf ./objs/*.o *.bin *.elf

.PHONY: indent
indent:
	clang-format -i $(SRCS_C) include/*.h
