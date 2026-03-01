package kernel32

import "console"
import "mb1"

import "base:runtime"

@(export)
_start :: proc "c" () -> ! {
	context = runtime.default_context()

	// Initializes globals and calls @(init) functions.
	#force_no_inline runtime._startup_runtime()

	main()

	for {

	}
}


main :: proc() {
	console := console.make_vga_console()
	console->clear()
	console->write("Hello, kernel!")
}
