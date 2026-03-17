#ifndef LOOM_ARCH_H
#define LOOM_ARCH_H 1

#include "loom/compiler.h"
#include "loom/types.h"

#define IA32_TSC_AUX 0xC0000103

typedef u64 cpu_handle;

static inline u64 force_inline
readMsr (u32 msr_id)
{
  u32 low, high;
  __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr_id) : "memory");
  return (u64) low | ((u64) high << 32);
}

static inline void force_inline
writeMsr (u32 msr_id, u64 value)
{
  __asm__ volatile ("wrmsr" ::"a"(value), "c"(msr_id), "d"(value >> 32)
                    : "memory");
}

static inline void force_inline
cpuHalt (void)
{
  __asm__ volatile ("halt" ::: "memory");
}

static inline u64 force_inline
cpuHandle (void)
{
  return readMsr (IA32_TSC_AUX);
}

static inline void force_inline
cpuRelax (void)
{
  __asm__ volatile ("pause");
}

static inline void force_inline
cpuStop (void)
{
  __asm__ volatile ("cli; halt" ::: "memory");
}

static inline void force_inline
sti (void)
{
  __asm__ volatile ("sti" ::: "memory");
}

static inline void force_inline
cli (void)
{
  __asm__ volatile ("cli" ::: "memory");
}

static inline ulong force_inline
irqSave (void)
{
  ulong flags;
  __asm__ volatile ("pushfq; popq %0; cli" : "=rm"(flags)::"memory");
  return flags;
}

static inline void force_inline
irqRestore (ulong flags)
{
  __asm__ volatile ("pushq %0; popfq" ::"rm"(flags) : "memory");
}

#endif