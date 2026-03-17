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

#define lockGet(lock) _Generic ((lock), spinlock *: spinLock) (lock)
#define lockGetIrq(lock, flags)                                               \
  _Generic ((lock), spinlock *: spinLockIrq) (lock, flags)

#define lockTry(lock) _Generic ((lock), spinlock *: spinTryLock) (lock)
#define lockTryIrq(lock, flags)                                               \
  _Generic ((lock), spinlock *: spinTryLockIrq) (lock, flags)

#define lockPut(lock) _Generic ((lock), spinlock *: spinUnlock) (lock)
#define lockPutIrq(lock, flags)                                               \
  _Generic ((lock), spinlock *: spinUnlockIrq) (lock, flags)

#define spinLockIrq(lock, flags)                                              \
  do                                                                          \
    {                                                                         \
      flags = irqSave ();                                                     \
      spinLock (lock);                                                        \
    }                                                                         \
  while (0)

#define spinTryLockIrq(lock, flags)                                           \
  ({                                                                          \
    flags = irqSave ();                                                       \
    auto _locked = spinTryLock (lock);                                        \
    if (!_locked)                                                             \
      irqRestore (flags);                                                     \
    _locked                                                                   \
  })

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