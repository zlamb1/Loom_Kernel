#include <stdatomic.h>

#include "loom/arch.h"
#include "loom/sync.h"

void
spin_lock (spinlock *lock)
{
  auto ticket
      = atomic_fetch_add_explicit (&lock->next, 1, memory_order_relaxed);

  for (;;)
    {
      auto owner = atomic_load_explicit (&lock->owner, memory_order_relaxed);
      if (ticket == owner)
        break;
      cpu_relax ();
    }

  atomic_thread_fence (memory_order_acquire);
}

bool
spin_trylock (spinlock *lock)
{
  auto next = atomic_load_explicit (&lock->next, memory_order_relaxed);
  auto owner = atomic_load_explicit (&lock->owner, memory_order_relaxed);

  if (owner != next)
    return false;

  return atomic_compare_exchange_strong_explicit (&lock->next, &next, next + 1,
                                                  memory_order_acquire,
                                                  memory_order_relaxed);
}

void
spin_unlock (spinlock *lock)
{
  atomic_fetch_add_explicit (&lock->owner, 1, memory_order_release);
}