SRCDIR := kernel
ARCHDIR := $(SRCDIR)/arch/x86
OUTDIR ?= build

CROSS_CC ?= i686-elf-gcc
CROSS_LD ?= i686-elf-ld

ASM_SRCS := $(ARCHDIR)/_start32.S $(ARCHDIR)/arch.S $(ARCHDIR)/arch32.S $(ARCHDIR)/io.S $(ARCHDIR)/multiboot1.S
ASM_OBJS := $(foreach src,$(ASM_SRCS),$(OUTDIR)/$(src:.S=.o))

KERNEL_LNK := $(SRCDIR)/arch/x86/link.ld
KERNEL_OBJ := $(OUTDIR)/kernel.o
KERNEL_ELF := $(OUTDIR)/kernel.elf

ODIN ?= odin
ODIN_FLAGS ?=
ODIN_FLAGS += -o:size -debug -target:linux_i386 -build-mode:obj -no-crt \
			  -no-entry-point -no-thread-local -default-to-panic-allocator \
			  -disable-red-zone -collection:kernel=$(SRCDIR) -out:$(KERNEL_OBJ)

QEMU ?= qemu-system-i386
QEMU_FLAGS ?=
QEMU_FLAGS += -kernel $(KERNEL_ELF)

GDB ?= gdb

.PHONY: all $(KERNEL_OBJ) run clean
 
all: $(KERNEL_ELF)

$(KERNEL_ELF): $(KERNEL_LNK) $(ASM_OBJS) $(KERNEL_OBJ)
	$(CROSS_LD) -T $(KERNEL_LNK) $(ASM_OBJS) $(KERNEL_OBJ) -m elf_i386 --oformat elf32-i386 -o $@

$(KERNEL_OBJ):
	$(ODIN) build $(SRCDIR) $(ODIN_FLAGS)

$(OUTDIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(CROSS_CC) -g -r $< -Wl,-z noexecstack -o $@

run: $(KERNEL_ELF)
	$(QEMU) $(QEMU_FLAGS)

dbg: $(KERNEL_ELF)
	$(QEMU) $(QEMU_FLAGS) -s -S &
	$(GDB) $< -q -ex 'set pagination off' -ex 'set confirm off' -ex 'target remote localhost:1234' -ex 'layout src' -ex 'b _start' -ex 'c'

clean:
	rm -rf $(OUTDIR)