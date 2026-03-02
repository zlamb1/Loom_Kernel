package mm

AllocatorError :: enum byte {
	None            = 0,
	OutOfMemory     = 1,
	InvalidPointer  = 2,
	InvalidArgument = 3,
}

MemoryRange :: struct {
	start: uintptr,
	end:   uintptr, // exclusive
}
