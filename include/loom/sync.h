#ifndef LOOM_SYNC_H
#define LOOM_SYNC_H 1

#include "loom/arch.h"
#include "loom/compiler.h"
#include "loom/types.h"

struct tspinlock
{
  atomic uint owner;
  atomic uint next;
};

typedef struct tspinlock spinlock;

#define spin_lock_irq(lock, flags)                                            \
  do                                                                          \
    {                                                                         \
      flags = irq_save ();                                                    \
      spin_lock (lock);                                                       \
    }                                                                         \
  while (0)

#define spin_unlock_irq(lock, flags)                                          \
  do                                                                          \
    {                                                                         \
      spin_unlock (lock);                                                     \
      irq_restore (flags);                                                    \
    }                                                                         \
  while (0)

void spin_lock (spinlock *lock);
bool spin_trylock (spinlock *lock);

void spin_unlock (spinlock *lock);

#endif