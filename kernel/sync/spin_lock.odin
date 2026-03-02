package sync

import "base:intrinsics"
import "kernel:arch"
import "kernel:cfg"
import "kernel:types"

when cfg.SMP {

	SpinLock :: struct {
		owner:  types.uhalf,
		next:   types.uhalf,
		flags:  uint,
		cpu_id: arch.cpu_id,
	}

	CacheLineSpinLock :: struct #align (arch.CACHE_LINE) {
		spin_lock: SpinLock,
		_:         [arch.CACHE_LINE - size_of(SpinLock)]byte,
	}

	#assert(align_of(CacheLineSpinLock) == arch.CACHE_LINE)
	#assert(size_of(CacheLineSpinLock) == arch.CACHE_LINE)

	/* 
		        A fair ticket spin lock implementation. 
		-----------------------------------------------------
		owner := The current owner.
		next := The next ticket.

		Lockers must fetch and add next atomically. The value
		they fetched is now their ticket and they must spin
		until owner == ticket.

		Lock owners must increment owner on unlock. This
		creates a fair FIFO queue. It has higher
		latency for low-contention cases compared to a CAS
		test-and-set loop. In exchange, it provides fairness.

		Wrap is safe since both owner and next will wrap. The only 
		potential for bad behavior is if the number of cores contending
		the lock exceeds the limit of uhalf.
	*/

	spin_lock_create :: proc() -> (lock: SpinLock) {
		lock.cpu_id = arch.CPU_ID_RESERVED
		return
	}

	spin_lock :: proc(self: ^SpinLock) {
		flags := arch.irq_save()
		cpu_id := arch.get_cpu_id()

		// Ordering of the increment doesn't matter to the current thread of execution.
		ticket := intrinsics.atomic_add_explicit(&self.next, 1, .Relaxed)

		// Same as above.
		owner := intrinsics.atomic_load_explicit(&self.owner, .Relaxed)

		if ticket != owner {
			// We have to spin. Check for deadlock, in case we are the owner of this lock.
			owner_cpu_id := intrinsics.atomic_load_explicit(&self.cpu_id, .Relaxed)
			if owner_cpu_id == cpu_id {
				panic("spin_lock: deadlock detected")
			}
		}

		for {
			// Reload lock in an atomically relaxed fashion to prevent any tearing or compiler reordering.
			owner = intrinsics.atomic_load_explicit(&self.owner, .Relaxed)

			if ticket == owner do break

			// This is merely empirical.
			backoff := (ticket - owner) * 10

			for i in 0 ..< backoff do arch.cpu_spinning()
		}

		// Do not reorder to before we acquired the lock.
		intrinsics.atomic_thread_fence(.Acquire)

		// Atomic store is unnecessary since we only observe flag as the lock holder.
		self.flags = flags
		intrinsics.atomic_store_explicit(&self.cpu_id, cpu_id, .Relaxed)
	}

	spin_unlock :: proc(self: ^SpinLock) {
		when cfg.DBG {
			cpu_id := arch.get_cpu_id()
			owner_cpu_id := intrinsics.atomic_load_explicit(&self.cpu_id, .Relaxed)

			if cpu_id != owner_cpu_id {
				panic("spin_unlock: unlocked while not held")
			}
		}

		// Save locks before unlocking so we don't race.
		flags := self.flags

		// Reset cpu_id so we don't detect a false deadlock if we quickly reacquire the lock.
		intrinsics.atomic_store_explicit(&self.cpu_id, arch.CPU_ID_RESERVED, .Relaxed)

		// Increment owner. Next ticket holder can now unlock.
		intrinsics.atomic_add_explicit(&self.owner, 1, .Release)

		// Restore IRQs after releasing lock.
		arch.irq_restore(flags)
	}

} else {

	SpinLock :: struct {
		flags: uint,
		held:  bool,
	}

	CacheLineSpinLock :: struct {
		spin_lock: SpinLock,
	}

	spin_lock_create :: proc "contextless" () -> (lock: SpinLock) {
		return
	}

	spin_lock :: proc(self: ^SpinLock) {
		flags := arch.irq_save()

		if (self.held) {
			panic("spin_lock: deadlock detected")
		}

		self.flags = flags
		self.held = true

		intrinsics.atomic_signal_fence(.Acquire)
	}

	spin_unlock :: proc(self: ^SpinLock) {
		when cfg.DBG {
			if !self.held {
				panic("spin_unlock: unlocked while not held")
			}
		}

		intrinsics.atomic_signal_fence(.Release)
		self.held = false
		arch.irq_restore(self.flags)
	}

}

cache_line_spin_lock_create :: proc "contextless" () -> CacheLineSpinLock {
	return {spin_lock = spin_lock_create()}
}

cache_line_spin_lock :: proc(self: ^CacheLineSpinLock) {
	spin_lock(&self.spin_lock)
}

cache_line_spin_unlock :: proc(self: ^CacheLineSpinLock) {
	spin_unlock(&self.spin_lock)
}
