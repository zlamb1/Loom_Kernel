package mm

MemoryType :: enum byte {
	Free     = 0,
	Reserved = 1,
}

MemoryRegion :: struct {
	start:  uint,
	length: uint,
	type:   MemoryType,
}

MemoryMap :: struct {
	regions: []MemoryRegion,
}

MemoryMapIterator :: struct {
	iterate:   proc(self: ^MemoryMapIterator) -> (MemoryRegion, bool),
	overrides: []MemoryRegion,
}

memory_map_hook :: proc(
	self: ^MemoryMapIterator,
	hook: proc(region: MemoryRegion) -> bool,
) -> bool {


	return true
}
