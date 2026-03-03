package page

import "kernel:arch"
import "kernel:cfg"
import "kernel:mm/allocator"

PAGE_SIZE :: arch.PAGE_SIZE

Allocator_Proc :: proc(
	self: ^Allocator,
	mode: allocator.Allocator_Mode,
	old_ptr: rawptr,
) -> (
	p: []byte,
	error: allocator.Allocator_Error,
)

Allocator :: struct {
	free_pages: uint,
	procedure:  Allocator_Proc,
	data:       rawptr,
}

alloc :: proc(self: ^Allocator) -> ([]byte, allocator.Allocator_Error) {
	cfg.dbg_assert(self != nil)
	cfg.dbg_assert(self.procedure != nil)
	return self->procedure(.Alloc, nil)
}

free :: proc(self: ^Allocator, old_ptr: rawptr) {
	cfg.dbg_assert(self != nil)
	cfg.dbg_assert(self.procedure != nil)
	self->procedure(.Free, old_ptr)
}
