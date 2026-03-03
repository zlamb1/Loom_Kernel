package page

import "core:slice"
import "kernel:cfg"
import "kernel:mm/allocator"

@(private = "file")
Node :: struct {
	next: ^Node,
}

List_Allocator :: struct {
	using super: Allocator,
	pages:       ^Node,
}

list_allocator_init :: proc(self: ^List_Allocator) {
	self.procedure = list_allocator_proc
	self.data = self
}

list_allocator_proc :: proc(
	self: ^Allocator,
	mode: allocator.Allocator_Mode,
	old_ptr: rawptr,
) -> (
	p: []byte,
	err: allocator.Allocator_Error,
) {
	cfg.dbg_assert(self != nil)
	cfg.dbg_assert(self.data != nil)
	self := cast(^List_Allocator)self.data

	switch mode {
	case .Alloc:
		if self.pages == nil {
			err = .Out_Of_Memory
			return
		}
		node := self.pages
		self.pages = node.next
		p = slice.from_ptr(cast(^byte)node, PAGE_SIZE)
	case .Free:
		addr := uintptr(old_ptr)
		if addr & (PAGE_SIZE - 1) > 0 do panic("misaligned pointer")
		node := cast(^Node)old_ptr
		node.next = self.pages
		self.pages = node
	}

	err = .None
	return
}
