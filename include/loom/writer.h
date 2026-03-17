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
writeNil (void *data, uint n, const char *s)
{
  return n;
}

static inline uint
writeChar (struct writer writer, char ch)
{
  return writer.write (writer.data, 1, &ch);
}

static inline uint
writeStr (struct writer writer, uint n, const char *s)
{
  return writer.write (writer.data, n, s);
}

static inline uint
writeCStr (struct writer writer, const char *s)
{
  uint n = 0;
  while (s[n] != '\0')
    ++n;
  return writer.write (writer.data, n, s);
}

static inline uint
writePad (struct writer writer, uint repeat, char ch)
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
      written += writeStr (writer, write, buf);
      repeat -= write;
    }

#undef CAP
  return written;
}

#endif