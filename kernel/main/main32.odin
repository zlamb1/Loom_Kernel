package main

import "kernel:console"

kmain32 :: proc() {
	console := console.make_vga_console()
	console->clear()
	console->write("Hello, kernel!")
}
