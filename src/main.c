#include "loom/console.h"
#include "loom/limine.h"
#include "loom/print.h"
#include "loom/time.h"

static struct console *early_console;

static void
kprint_init (void)
{
  u64                         framebuffer_count;
  struct limine_framebuffer **framebuffers;
  struct limine_framebuffer  *framebuffer;

  if (!limine_get_framebuffers (&framebuffer_count, &framebuffers)
      || !framebuffer_count)
    return;

  framebuffer = framebuffers[0];
  if (framebuffer == null)
    return;

  early_console = early_limine_gfx_console_create (framebuffer);
  set_print_console (early_console);
}

static void
limine_early_boot (void)
{
  timestamp boot_time;
  kprint_init ();
  kprintfln ("Booting...");
  if (limine_get_boot_time (&boot_time))
    ts_kprintfln (boot_time, "Boot Time: %A, %B %e, %Y, %I:%M:%S %P UTC");
}

noreturn void
loom_main (void)
{
  limine_early_boot ();

  for (;;)
    ;
}