package console

import "base:intrinsics"
import "core:slice"

import "kernel:arch/x86"

@(private = "file")
Vga_Color :: bit_field u8 {
	fg: u8 | 4,
	bg: u8 | 4,
}

@(private = "file")
Vga_Attributes :: struct #packed {
	ch:           u8,
	using colors: Vga_Color,
}

@(private = "file")
Vga_Console :: struct {
	using super: Console,
	x:           u32,
	y:           u32,
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
p := slice.from_ptr(cast(^Vga_Attributes)uintptr(0xB8000), CELLS)

@(private = "file")
move_cursor :: proc(x: u32, y: u32) {
	pos: u16 = u16(y * COLS + x)
	x86.outb(CRTC_ADDRESS_REG, CRTC_CURSOR_LOW_LOC_REG)
	x86.outb(CRTC_DATA_REG, u8(pos & 0xFF))
	x86.outb(CRTC_ADDRESS_REG, CRTC_CURSOR_HIGH_LOC_REG)
	x86.outb(CRTC_DATA_REG, u8((pos >> 8) & 0xFF))
}

@(private = "file")
vga_console_proc :: proc(
	self: ^Console,
	mode: Console_Proc_Mode,
	args: Console_Proc_Args,
) -> (
	ret: Console_Proc_Return,
	err: Console_Error,
) {
	self := cast(^Vga_Console)self

	switch mode {
	case .Get_WH:
		ret.wh[0] = COLS
		ret.wh[1] = ROWS
	case .Get_XY:
		ret.xy[0] = self.x
		ret.xy[1] = self.y
	case .Write:
		vga_console_write(self, args.s)
	case .Clear:
		vga_console_clear(self)
	}

	err = .None
	return
}

@(private = "file")
vga_console_write :: proc(self: ^Vga_Console, s: []u8) {
	index: u32 = self.y * COLS + self.x

	for i in 0 ..< len(s) {
		if index >= CELLS do index = 0
		ch := s[i]
		intrinsics.volatile_store(&p[index], {ch = ch, fg = FG, bg = BG})
		index += 1
	}

	self.x = index % COLS
	self.y = index / COLS
	move_cursor(self.x, self.y)
}

@(private = "file")
vga_console_clear :: proc(self: ^Vga_Console) {
	for index in 0 ..< CELLS {
		intrinsics.volatile_store(&p[index], {ch = 0, fg = FG, bg = BG})
	}

	self.x = 0
	self.y = 0
	move_cursor(0, 0)
}

@(private = "file")
vga_console := Vga_Console {
	procedure = vga_console_proc,
}

vga_console_get :: proc() -> ^Console {
	return &vga_console.super
}
