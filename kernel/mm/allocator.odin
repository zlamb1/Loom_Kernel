package mm

import "allocator"
import "page"

Allocator_Error :: allocator.Allocator_Error
Allocator_Mode :: allocator.Allocator_Mode

alloc :: proc {
	page.alloc,
}

free :: proc {
	page.free,
}
