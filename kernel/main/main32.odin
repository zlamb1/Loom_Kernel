package main

import "kernel:console"
import "kernel:fmt"

kmain32 :: proc() {
	console := console.make_vga_console()
	console->clear()
	console->write("Hello, kernel!")
}
