#include "loom/print.h"
#include "loom/writer.h"

static struct console *print_console = null;

static inline uint
write_nil (void *, uint n, const char *)
{
  return n;
}

struct console *
get_print_console (void)
{
  return print_console;
}

void
set_print_console (struct console *console)
{
  print_console = console;
}

uint
vwprintf (struct writer writer, const char *fmt, va_list args)
{
  byte b;
  uint n = 0, i = 0;

  if (writer.write == null)
    writer.write = write_nil;

  for (;; ++i)
    {
      byte b = fmt[i];
      uint start = i;

      while (b != '\0' && b != '%')
        b = fmt[++i];

      if (i > start)
        n += writer.write (writer.data, i - start, fmt + start);

      if (b == '\0')
        break;

      b = fmt[++i];

      if (b == '%')
        {
          n += write_char (writer, '%');
          continue;
        }

      switch (b)
        {
        case 'c':
          n += write_char (writer, va_arg (args, int));
          break;
        case 's':
          n += write_cstr (writer, va_arg (args, const char *));
          break;
        case 'i':
        case 'd':
          n += write_int (writer, va_arg (args, int));
          break;
        case 'u':
          n += write_uint (writer, va_arg (args, uint));
          break;
        }
    }

  va_end (args);

  return n;
}
