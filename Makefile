CC      = gcc
LD      = ld
CFLAGS  = -m32 -static -ggdb -MD -Wall -Werror -I./include -O2 \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -fgnu89-inline
ASFLAGS = -m32 -MD -I./include
LDFLAGS = -melf_i386
QEMU    = qemu-system-i386

CFILES  = $(shell find src/ -name "*.c")
SFILES  = $(shell find src/ -name "*.S")
OBJS    = $(CFILES:.c=.o) $(SFILES:.S=.o)

run: disk.img
	$(QEMU) -serial stdio disk.img

log: disk.img
	$(QEMU) -serial file:./log.out disk.img

debug: disk.img
	$(QEMU) -serial stdio -s -S disk.img

disk.img: kernel
	@cd boot; make
	cat boot/bootblock kernel > disk.img

kernel: $(OBJS)
	$(LD) $(LDFLAGS) -e os_init -Ttext 0xC0100000 -o kernel $(OBJS)
	objdump -D kernel > code.txt	# disassemble result
	readelf -a kernel > elf.txt		# obtain more information about the executabl

-include $(OBJS:.o=.d)

count:
	@echo all lines including ^$$:
	@find . -name "*.[chS]" | xargs grep ".*" | wc -l
	@echo all lines without ^$$:
	@find . -name "*.[chS]" | xargs grep "." | wc -l

clean:
	@cd boot; make clean
	rm -f kernel disk.img $(OBJS) $(OBJS:.o=.d)

