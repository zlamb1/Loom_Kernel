package arch

@(linkage = "weak")
cpu_spinning :: proc "c" () {
	// Used for specific architectures as a way
	// to notify the CPU that we are spinning.
}

cpu_id :: distinct uint

CPU_ID_RESERVED :: max(cpu_id)

get_cpu_id :: proc() -> cpu_id {
	return 0
}

foreign _ {
	irq_enable :: proc "c" () ---
	irq_disable :: proc "c" () ---
	irq_save :: proc "c" () -> uint ---
	irq_restore :: proc "c" (flags: uint) ---
}
