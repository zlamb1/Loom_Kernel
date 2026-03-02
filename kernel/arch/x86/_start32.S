.text

.extern _start_odin
.extern sstack

.global _start

_start:
    mov $0, %ebp
    mov $sstack, %esp
    jmp _start_odin