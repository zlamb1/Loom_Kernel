ENTRY(_start)

SECTIONS {

    . = 0x100000;

    .multiboot ALIGN(0x1000) : {
        *(.multiboot)
    }

    .text ALIGN(0x1000) : {
        stext = .;
        *(.text*)
        etext = .;
    }

    .data ALIGN(0x1000) : {
        sdata = .;
        *(.data*)
        edata = .;
    }

    .rodata ALIGN(0x1000) : {
        srodata = .;
        *(.rodata*)
        erodata = .;
    }

    .bss ALIGN(0x1000) : {
        sbss = .;
        *(.bss*)
        ebss = .;
    }

    .stack ALIGN(0x1000) : {
        sstack = .;
        . += 0x2000;
        estack = .;
    }

}
