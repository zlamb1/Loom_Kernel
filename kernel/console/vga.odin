package console

import "base:intrinsics"
import "core:slice"

import "kernel:arch/x86"

@(private = "file")
VgaColor :: bit_field u8 {
	fg: u8 | 4,
	bg: u8 | 4,
}

@(private = "file")
VgaAttributes :: struct #packed {
	ch:     u8,
	colors: VgaColor,
}

@(private = "file")
VgaConsole :: struct {
	super: Console,
	x:     u32,
	y:     u32,
}

@(private = "file")
ROWS :: 25

@(private = "file")
COLS :: 80

@(private = "file")
CELLS :: ROWS * COLS

@(private = "file")
FG :: u8(0xF)

@(private = "file")
BG :: u8(0x0)

@(private = "file")
CRTC_ADDRESS_REG :: u16(0x3D4)

@(private = "file")
CRTC_DATA_REG :: u16(0x3D5)

@(private = "file")
CRTC_CURSOR_HIGH_LOC_REG :: u8(0xE)

@(private = "file")
CRTC_CURSOR_LOW_LOC_REG :: u8(0xF)

@(private = "file")
p := slice.from_ptr(cast(^VgaAttributes)uintptr(0xB8000), CELLS)

@(private = "file")
make_color :: proc(fg := FG, bg := BG) -> VgaColor {
	return {fg = fg, bg = bg}
}

@(private = "file")
move_cursor :: proc(x: u32, y: u32) {
	pos: u16 = u16(y * COLS + x)
	x86.outb(CRTC_ADDRESS_REG, CRTC_CURSOR_LOW_LOC_REG)
	x86.outb(CRTC_DATA_REG, u8(pos & 0xFF))
	x86.outb(CRTC_ADDRESS_REG, CRTC_CURSOR_HIGH_LOC_REG)
	x86.outb(CRTC_DATA_REG, u8((pos >> 8) & 0xFF))
}

@(private = "file")
vga_console_getwh :: proc(self: ^Console) -> (w: u32, h: u32) {
	w = COLS
	h = ROWS
	return
}

@(private = "file")
vga_console_getxy :: proc(self: ^Console) -> (x: u32, y: u32) {
	self := cast(^VgaConsole)self
	x = self.x
	y = self.y
	return
}

@(private = "file")
vga_console_write :: proc(self: ^Console, s: string) {
	self := cast(^VgaConsole)self
	index: u32 = self.y * COLS + self.x
	default := make_color()

	for i in 0 ..< len(s) {
		if index >= CELLS do index = 0
		ch := s[i]
		intrinsics.volatile_store(&p[index], {ch = ch, colors = default})
		index += 1
	}

	self.x = index % COLS
	self.y = index / COLS
	move_cursor(self.x, self.y)
}

@(private = "file")
vga_console_clear :: proc(self: ^Console) {
	self := cast(^VgaConsole)self
	default := make_color()

	for index in 0 ..< CELLS {
		intrinsics.volatile_store(&p[index], {ch = 0, colors = default})
	}

	self.x = 0
	self.y = 0
	move_cursor(0, 0)
}

@(private = "file")
vga_console := VgaConsole {
	super = {
		getwh = vga_console_getwh,
		getxy = vga_console_getxy,
		write = vga_console_write,
		clear = vga_console_clear,
	},
}

make_vga_console :: proc() -> ^Console {
	return &vga_console.super
}
