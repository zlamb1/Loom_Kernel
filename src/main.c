#include "loom/compiler.h"
#include "loom/console.h"
#include "loom/limine.h"
#include "loom/print.h"

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

noreturn void
loom_main (void)
{
  kprint_init ();
  kprintf ("Hello, kernel! %d\n", -123);

  for (;;)
    ;
}