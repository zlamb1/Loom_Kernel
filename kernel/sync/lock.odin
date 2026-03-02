package sync

lock :: proc {
	spin_lock,
	cache_line_spin_lock,
}

unlock :: proc {
	spin_unlock,
	cache_line_spin_unlock,
}
