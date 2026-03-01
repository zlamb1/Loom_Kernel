package console

Console :: struct {
	getwh: proc(self: ^Console) -> (w: u32, h: u32),
	getxy: proc(self: ^Console) -> (x: u32, y: u32),
	write: proc(self: ^Console, s: string),
	clear: proc(self: ^Console),
}
