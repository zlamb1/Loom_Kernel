package console

import "kernel:cfg"
import "kernel:types"

Console_Proc_Mode :: enum byte {
	Get_WH,
	Get_XY,
	Write,
	Clear,
}

Console_Proc_Args :: struct #raw_union {
	s: []u8,
}

Console_Proc_Return :: struct #raw_union {
	wh: [2]u32,
	xy: [2]u32,
}

Console_Error :: enum byte {
	None,
	Mode_Not_Supported,
}

Console_Proc :: proc(
	self: ^Console,
	mode: Console_Proc_Mode,
	args: Console_Proc_Args,
) -> (
	Console_Proc_Return,
	Console_Error,
)

Console :: struct {
	procedure: Console_Proc,
	data:      rawptr,
	node:      types.List_Node,
}

get_wh :: proc(self: ^Console) -> (w: u32, h: u32, err: Console_Error) {
	cfg.dbg_assert(self != nil)
	cfg.dbg_assert(self.procedure != nil)
	ret: Console_Proc_Return
	ret, err = self->procedure(.Get_WH, {})
	if err == .None {
		w = ret.wh[0]
		h = ret.wh[1]
	}
	return
}

get_xy :: proc(self: ^Console) -> (x: u32, y: u32, err: Console_Error) {
	cfg.dbg_assert(self != nil)
	cfg.dbg_assert(self.procedure != nil)
	ret: Console_Proc_Return
	ret, err = self->procedure(.Get_XY, {})
	if err == .None {
		x = ret.xy[0]
		y = ret.xy[1]
	}
	return
}

write :: proc(self: ^Console, s: []u8) -> (err: Console_Error) {
	cfg.dbg_assert(self != nil)
	cfg.dbg_assert(self.procedure != nil)
	_, err = self->procedure(.Write, {s = s})
	return
}

clear :: proc(self: ^Console) -> (err: Console_Error) {
	cfg.dbg_assert(self != nil)
	cfg.dbg_assert(self.procedure != nil)
	_, err = self->procedure(.Clear, {})
	return
}
