#ifndef LOOM_WRITER_H
#define LOOM_WRITER_H 1

#include <limits.h>

#include "loom/types.h"

struct writer
{
  uint (*write) (void *, uint, const char *);
  void *data;
};

static inline uint
write_char (struct writer writer, char ch)
{
  return writer.write (writer.data, 1, &ch);
}

static inline uint
write_str (struct writer writer, uint n, const char *s)
{
  return writer.write (writer.data, n, s);
}

static inline uint
write_cstr (struct writer writer, const char *s)
{
  uint n = 0;
  while (s[n] != '\0')
    ++n;
  return writer.write (writer.data, n, s);
}

static inline uint
write_pad (struct writer writer, uint repeat, char ch)
{
#define CAP 64
  char buf[CAP];
  uint written = 0;

  for (uint i = 0; i < CAP; ++i)
    buf[i] = ch;

  while (repeat)
    {
      uint write = CAP;
      if (repeat < write)
        write = repeat;
      written += write_str (writer, write, buf);
      repeat -= write;
    }

#undef CAP
  return written;
}

#endif