package mm

Memory_Type :: enum byte {
	Free     = 0,
	Reserved = 1,
}

Memory_Region :: struct {
	start:  uint,
	length: uint,
	type:   Memory_Type,
}

Memory_Map :: struct {
	regions: []Memory_Region,
}
