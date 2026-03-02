package fmt

import "kernel:cfg"
import "kernel:console"
import "kernel:sync"
import "kernel:types"

Fmt :: struct {
	lock:     sync.SpinLock,
	consoles: types.ListNode,
}

fmt := Fmt{}

@(init)
fmt_init :: proc "contextless" () {
	fmt.lock = sync.spin_lock_create()
	types.list_init(&fmt.consoles)
}

console_register :: proc(console: ^console.Console) {
	cfg.dbg_assert(console != nil)
	sync.lock(&fmt.lock)
	types.list_add(&fmt.consoles, &console.node)
	sync.unlock(&fmt.lock)
}

console_unregister :: proc(console: ^console.Console) {
	cfg.dbg_assert(console != nil)
	sync.lock(&fmt.lock)
	types.list_remove(&console.node)
	sync.unlock(&fmt.lock)
}

printf :: proc(fmt: string, args: any) {
}
