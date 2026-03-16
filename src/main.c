#include "loom/limine.h"
#include "loom/mmap.h"
#include "loom/print.h"

noreturn void
loom_main (void)
{
  auto bi = limine_early_boot ();

  kprintfln ("Booting...");

  if (bi.boot_time_set)
    ts_kprintfln (bi.boot_time, "Boot Time: %A, %B %e, %Y, %I:%M:%S %P UTC");

  if (bi.mmap_iterator != null)
    mmap_init (bi.mmap_iterator);

  for (;;)
    ;
}