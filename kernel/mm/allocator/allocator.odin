package allocator

Allocator_Error :: enum byte {
	None             = 0,
	Out_Of_Memory    = 1,
	Invalid_Pointer  = 2,
	Invalid_Argument = 3,
}

Allocator_Mode :: enum byte {
	Alloc,
	Free,
}
