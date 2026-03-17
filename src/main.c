#include <stdatomic.h>

#include "loom/arch.h"
#include "loom/limine.h"
#include "loom/mmap.h"
#include "loom/print.h"
#include "loom/time.h"

static atomic bool mpPause = true;

noreturn void
mpMain (void)
{
  while (atomic_load_explicit (&mpPause, memory_order_relaxed))
    cpuRelax ();

  kLogLn ("Starting MP #%u64", cpuHandle ());

  cpuStop ();

  for (;;)
    ;
}

noreturn void
loomMain (void)
{
  struct boot_request rq = { .bs_func = mpMain };
  auto                bi = limineEarlyBoot (rq);

  kLogLn ("Booting...");

  if (bi.boot_time_set)
    tsLogLn (bi.boot_time, "Boot Time: %A, %B %e, %Y, %I:%M:%S %P UTC");

  if (bi.mmap_iterator != null)
    mmapInit (bi.mmap_iterator);

  kLogLn ("Bootstrap MP #%u64", cpuHandle ());

  atomic_store_explicit (&mpPause, false, memory_order_relaxed);

  for (;;)
    ;
}