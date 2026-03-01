.text

.global inb
.global outb

inb:
    movw 4(%esp), %dx
    inb %dx, %al
    ret

outb:
    movw 4(%esp), %dx
    movb 8(%esp), %al
    outb %al, %dx
    ret