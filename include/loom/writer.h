#ifndef LOOM_WRITER_H
#define LOOM_WRITER_H 1

#include <limits.h>

#include "loom/types.h"

struct writer
{
  uint (*write) (void *, uint, const char *);
  void *data;
};

#define DECLARE_WRITE_UNSIGNED(type)                                          \
  static inline type write_##type (struct writer writer, type u)              \
  {                                                                           \
    char buf[sizeof (type) * CHAR_BIT];                                       \
    int  digits = 0;                                                          \
                                                                              \
    if (u == 0)                                                               \
      {                                                                       \
        buf[0] = '0';                                                         \
        digits = 1;                                                           \
      }                                                                       \
                                                                              \
    while (u)                                                                 \
      {                                                                       \
        type r = u % 10;                                                      \
        u /= 10;                                                              \
        buf[digits++] = r + '0';                                              \
      }                                                                       \
                                                                              \
    for (int i = 0, j = digits - 1; i < (digits >> 1); ++i, --j)              \
      {                                                                       \
        char tmp = buf[i];                                                    \
        buf[i] = buf[j];                                                      \
        buf[j] = tmp;                                                         \
      }                                                                       \
                                                                              \
    return write_str (writer, digits, buf);                                   \
  }

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

DECLARE_WRITE_UNSIGNED (uint)

static inline uint
write_int (struct writer writer, int i)
{
  uint n = 0, u;

  if (i < 0)
    {
      n = write_char (writer, '-');
      if (i == INT_MIN)
        u = ((uint) INT_MAX) + 1;
      else
        u = (uint) (-i);
    }
  else
    u = (uint) i;

  return n + write_uint (writer, u);
}

#endif