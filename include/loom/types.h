#ifndef LOOM_TYPES_H
#define LOOM_TYPES_H 1

#include <stdint.h>

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef null
#define null 0
#endif

typedef unsigned char  byte;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef u8  b8;
typedef u16 b16;
typedef u32 b32;
typedef u64 b64;

typedef u8  u8le;
typedef u16 u16le;
typedef u32 u32le;
typedef u64 u64le;

typedef u8  u8be;
typedef u16 u16be;
typedef u32 u32be;
typedef u64 u64be;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef i8  i8le;
typedef i16 i16le;
typedef i32 i32le;
typedef i64 i64le;

typedef i8  i8be;
typedef i16 i16be;
typedef i32 i32be;
typedef i64 i64be;

typedef float       f32;
typedef double      f64;
typedef long double f80;

typedef uintptr_t uintptr;
typedef intptr_t  intptr;

#endif