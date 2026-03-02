package main

import "base:runtime"

@(export)
_start_odin :: proc "c" () -> ! {
	context = runtime.default_context()

	// Initializes globals and calls @(init) functions.
	#force_no_inline runtime._startup_runtime()

	kmain32()

	for {

	}
}
