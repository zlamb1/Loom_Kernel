package mm

PageAllocator :: struct {
	free_pages: uint,
	alloc:      proc(self: ^PageAllocator) -> ([]byte, AllocatorError),
	free:       proc(self: ^PageAllocator, p: rawptr),
	data:       rawptr,
}

@(private = "file")
Node :: struct {
	next: ^Node,
}

ListPageAllocator :: struct {
	super: PageAllocator,
	pages: ^Node,
}

list_page_allocator_init :: proc(self: ^ListPageAllocator) {
	self.super.alloc = list_page_allocator_alloc
	self.super.free = list_page_allocator_free
	self.super.data = self
}

list_page_allocator_alloc :: proc(self: ^PageAllocator) -> ([]byte, AllocatorError) {
	self := cast(^ListPageAllocator)self.data

	return nil, .None
}

list_page_allocator_free :: proc(self: ^PageAllocator, p: rawptr) {
	self := cast(^ListPageAllocator)self.data
}
