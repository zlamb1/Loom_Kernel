ENTRY(loom_main)

SECTIONS {
    __virt_base = 0xffffffff80000000;
    __phys_base = 0x100000;

    . = __virt_base;

    .text : AT(__phys_base) {
        *(.text*)
    }

    . = ALIGN(0x1000);

    .data : {
        *(.data*)
    }

    . = ALIGN(0x1000);

    .rodata : {
        *(.rodata*)
    }

    . = ALIGN(0x1000);

    .bss : {
        *(.bss*)
    }

    __phys_end = . - __virt_base + __phys_base;

    /DISCARD/ : {
        *(.eh_frame*)
    }
}