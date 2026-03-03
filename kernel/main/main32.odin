package main

import "kernel:console"
import "kernel:fmt"
import "kernel:mm/page"

kmain32 :: proc() {
	con := console.vga_console_get()
	console.clear(con)
	s := "Hello, Kernel!"
	console.write(con, transmute([]u8)s)
}
