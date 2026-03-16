#ifndef LOOM_ARCH_H
#define LOOM_ARCH_H 1

#include "loom/compiler.h"
#include "loom/types.h"

static inline void force_inline
cpu_relax (void)
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
irq_save (void)
{
  ulong flags;
  __asm__ volatile ("pushfq; popq %0; cli" : "=rm"(flags)::"memory");
  return flags;
}

static inline void force_inline
irq_restore (ulong flags)
{
  __asm__ volatile ("pushq %0; popfq" ::"rm"(flags) : "memory");
}

#endif