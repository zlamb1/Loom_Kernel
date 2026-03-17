#ifndef LOOM_ARCH_H
#define LOOM_ARCH_H 1

#include "loom/compiler.h"
#include "loom/types.h"

static inline void force_inline
cpuRelax (void)
{
  __asm__ volatile ("pause");
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