package types

/* _half variants are half the size of their _int counterparts. */

when size_of(uint) == 16 {
	uhalf :: distinct u64
} else when size_of(uint) == 8 {
	uhalf :: distinct u32
} else when size_of(uint) == 4 {
	uhalf :: distinct u16
} else when size_of(uint) == 2 {
	uhalf :: distinct u8
} else {
	#panic("unsupported uint size")
}
