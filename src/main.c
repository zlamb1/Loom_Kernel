#include "loom/limine.h"
#include "loom/mmap.h"
#include "loom/print.h"
#include "loom/time.h"

noreturn void
loomMain (void)
{
  auto bi = limineEarlyBoot ();

  kLogLn ("Booting...");

  if (bi.boot_time_set)
    tsLogLn (bi.boot_time, "Boot Time: %A, %B %e, %Y, %I:%M:%S %P UTC");

  if (bi.mmap_iterator != null)
    mmapInit (bi.mmap_iterator);

  for (;;)
    ;
}