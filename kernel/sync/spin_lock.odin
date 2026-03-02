package sync

import "base:intrinsics"
import "kernel:arch"

@(private)
SpinLock :: struct #align (arch.CACHE_LINE) {
	lock:   uint,
	flags:  uint,
	cpu_id: arch.cpu_id,
	_:      [arch.CACHE_LINE - size_of(uint) * 2 - size_of(arch.cpu_id)]byte,
}

#assert(align_of(SpinLock) == arch.CACHE_LINE)
#assert(size_of(SpinLock) == arch.CACHE_LINE)

@(private = "file")
BITSIZE :: size_of(uint) * 4

@(private = "file")
MASK :: (1 << BITSIZE) - 1

/* A fair ticket spin lock implementation. */

make_spin_lock :: proc() -> (lock: SpinLock) {
	lock.cpu_id = arch.CPU_ID_RESERVED
	return
}

spin_lock :: proc(self: ^SpinLock) {
	flags := arch.irq_save()
	cpu_id := arch.get_cpu_id()

	v := intrinsics.atomic_add_explicit(&self.lock, 1 << BITSIZE, .Acquire)
	owner := v & MASK
	ticket := (v >> BITSIZE) & MASK

	if ticket != owner {
		// We have to spin. Check for deadlock, in case we are the owner of this lock.
		owner_cpu_id := intrinsics.atomic_load_explicit(&self.cpu_id, .Relaxed)
		if owner_cpu_id == cpu_id {
			panic("spin_lock: deadlock detected")
		}
	}

	// Reload lock in an atomically relaxed fashion to prevent any tearing or compiler shenanigans.
	for ; ticket != owner; owner = intrinsics.atomic_load_explicit(&self.lock, .Relaxed) & MASK {
		// Notify CPU we are spinning. (e.g. pause on x86)
		arch.cpu_spinning()
	}

	// Don't reorder any memory operations to before we acquired the lock.
	intrinsics.atomic_thread_fence(.Acquire)

	self.flags = flags
	intrinsics.atomic_store_explicit(&self.cpu_id, cpu_id, .Relaxed)
}

spin_unlock :: proc(self: ^SpinLock) {
	intrinsics.atomic_store_explicit(&self.cpu_id, arch.CPU_ID_RESERVED, .Relaxed)
	// Add 1 to owner. Next ticket holder can now unlock.
	// Release semantics prevents any stores from escaping the critical section.
	intrinsics.atomic_add_explicit(&self.lock, 1, .Release)
	arch.irq_restore(self.flags)
}
