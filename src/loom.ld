ENTRY(loom_main)

SECTIONS {
    __virt_base = 0xffffffff80000000;
    __phys_base = 0x100000;

    . = __virt_base;

    .text : AT(. - __virt_base + __phys_base) {
        *(.text*)
    }

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .data : AT(. - __virt_base + __phys_base) {
        *(.data*)
    }

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .rodata : AT(. - __virt_base + __phys_base) {
        *(.rodata*)
    }

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .bss : AT(. - __virt_base + __phys_base) {
        *(.bss*)
    }

    /DISCARD/ : {
        *(.eh_frame*)
    }
}