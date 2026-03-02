.text

.global irq_enable
.global irq_disable
.global irq_save
.global irq_restore

irq_enable:
    sti
    ret

irq_disable:
    cli
    ret

irq_save:
    pushfl
    pop %eax
    ret

irq_restore:
    push 4(%esp)
    popfl
    ret