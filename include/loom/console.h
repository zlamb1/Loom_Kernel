#ifndef LOOM_CONSOLE_H
#define LOOM_CONSOLE_H 1

#include "loom/types.h"

struct console
{
  uint (*write) (void *, uint, const char *);
  void *data;
};

struct console *early_gfx_console_create (u8 *address, u32 width, u32 height,
                                          u32 stride, u32 bpp);

static inline uint
console_write (struct console *console, uint n, const char *s)
{
  if (console == null)
    return 0;

  return console->write (console->data, n, s);
}

#endif