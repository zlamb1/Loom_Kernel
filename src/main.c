#include "loom/compiler.h"
#include "loom/limine.h"

noreturn void
loom_main (void)
{
  u64                         framebuffer_count;
  struct limine_framebuffer **framebuffers;
  struct limine_framebuffer  *framebuffer;

  if (!limine_get_framebuffers (&framebuffer_count, &framebuffers)
      || !framebuffer_count)
    goto done;

  framebuffer = framebuffers[0];
  if (framebuffer == null)
    goto done;

  byte *data = framebuffer->address;
  data[0] = 0xFF;
  data[1] = 0xFF;
  data[2] = 0xFF;

done:
  for (;;)
    ;
}