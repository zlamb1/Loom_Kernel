SRCDIR := kernel32
OUTDIR ?= build

CROSS_CC ?= i686-elf-gcc
CROSS_LD ?= i686-elf-ld

ASM_SRCS := $(SRCDIR)/multiboot1.S $(SRCDIR)/arch/x86/io.S
ASM_OBJS := $(foreach src,$(ASM_SRCS),$(OUTDIR)/$(src:.S=.o))

KERNEL32_LNK := $(SRCDIR)/arch/x86/link.ld

KERNEL32_OBJ := $(OUTDIR)/kernel32.o
KERNEL32_ELF := $(OUTDIR)/kernel32.elf

ODIN ?= odin
ODIN_FLAGS ?=
ODIN_FLAGS += -o:size -debug -target:linux_i386 -build-mode:obj -no-crt \
			  -no-entry-point -no-thread-local -default-to-panic-allocator \
			  -disable-red-zone -collection:arch=$(SRCDIR)/arch -out:$(KERNEL32_OBJ)

QEMU ?= qemu-system-i386
QEMU_FLAGS ?=
QEMU_FLAGS += -kernel $(KERNEL32_ELF)

GDB ?= gdb

.PHONY: all $(KERNEL32_OBJ) run clean
 
all: $(KERNEL32_ELF)

$(KERNEL32_ELF): $(KERNEL32_LNK) $(ASM_OBJS) $(KERNEL32_OBJ)
	$(CROSS_LD) -T $(KERNEL32_LNK) $(ASM_OBJS) $(KERNEL32_OBJ) -m elf_i386 --oformat elf32-i386 -o $@

$(KERNEL32_OBJ):
	$(ODIN) build $(SRCDIR) $(ODIN_FLAGS)

$(OUTDIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(CROSS_CC) -g -r $< -Wl,-z noexecstack -o $@

run: $(KERNEL32_ELF)
	$(QEMU) $(QEMU_FLAGS)

dbg: $(KERNEL32_ELF)
	$(QEMU) $(QEMU_FLAGS) -s -S &
	$(GDB) $< -q -ex 'set pagination off' -ex 'set confirm off' -ex 'target remote localhost:1234' -ex 'layout src' -ex 'b _start' -ex 'c'

clean:
	rm -rf $(OUTDIR)