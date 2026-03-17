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

#define lockGet(lock)    _Generic ((lock), spinlock *: spinLockGet) (lock)
#define lockGetIrq(lock) _Generic ((lock), spinlock *: spinLockGetIrq) (lock)
#define lockGetIrq_S(lock, flags)                                             \
  _Generic ((lock), spinlock *: spinLockGetIrq_S) (lock, flags)

#define lockTry(lock) _Generic ((lock), spinlock *: spinLockTry) (lock)
#define lockTryIrq(lock, flags)                                               \
  _Generic ((lock), spinlock *: spinLockTryIrq) (lock, &flags)

#define lockPut(lock) _Generic ((lock), spinlock *: spinLockPut) (lock)
#define lockPutIrq(lock, flags)                                               \
  _Generic ((lock), spinlock *: spinLockPutIrq) (lock, &flags)

void spinLockGet (spinlock *lock);
bool spinLockTry (spinlock *lock);
void spinLockPut (spinlock *lock);

static inline ulong force_inline
spinLockGetIrq (spinlock *lock)
{
  auto flags = irqSave ();
  spinLockGet (lock);
  return flags;
}

static inline void force_inline
spinLockGetIrq_S (spinlock *lock, ulong *flags)
{
  *flags = irqSave ();
  spinLockGet (lock);
}

static inline bool force_inline
spinLockTryIrq (spinlock *lock, ulong *flags)
{
  *flags = irqSave ();
  auto locked = spinLockTry (lock);

  if (!locked)
    irqRestore (*flags);

  return locked;
}

static inline void force_inline
spinLockPutIrq (spinlock *lock, ulong *flags)
{
  spinLockPut (lock);
  irqRestore (*flags);
}

#endif