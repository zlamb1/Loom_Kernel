.section .multiboot, "a", @progbits

.equ MAGIC, 0x1BADB002
.equ FLAGS, 0x0

header:
    magic: .long MAGIC
    flags: .long FLAGS
    checksum: .long -(MAGIC + FLAGS)
    