package x86

foreign _ {
	inb :: proc "c" (port: u16) -> u8 ---
	outb :: proc "c" (port: u16, value: u8) ---
}
