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

#define spinLockIrq(lock, flags)                                              \
  do                                                                          \
    {                                                                         \
      flags = irqSave ();                                                     \
      spinLock (lock);                                                        \
    }                                                                         \
  while (0)

#define spinUnlockIrq(lock, flags)                                            \
  do                                                                          \
    {                                                                         \
      spinUnlock (lock);                                                      \
      irqRestore (flags);                                                     \
    }                                                                         \
  while (0)

void spinLock (spinlock *lock);
bool spinTryLock (spinlock *lock);
void spinUnlock (spinlock *lock);

#endif