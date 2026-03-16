#ifndef LOOM_COMPILER_H
#define LOOM_COMPILER_H 1

#define aligned(x)   __attribute__ ((aligned (x)))
#define auto         __auto_type
#define atomic       _Atomic
#define force_inline __attribute__ ((always_inline))
#define noreturn     __attribute__ ((noreturn))
#define packed       __attribute__ ((packed))
#define used         __attribute__ ((used))

#endif