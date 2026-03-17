#include "loom/print.h"
#include "loom/compiler.h"
#include "loom/writer.h"

#define SIZE_NONE 0
#define SIZE_8    1
#define SIZE_16   2
#define SIZE_32   3
#define SIZE_64   4

struct format
{
  bool minus : 1;
  bool plus : 1;
  bool space : 1;
  bool hash : 1;
  bool zero : 1;
  bool caret : 1;
  bool bang : 1;
  bool width_set : 1;
  bool prec_set : 1;
  uint width;
  uint prec;
};

struct arg
{
  bool ok;
  uint value, length;
};

struct int_size
{
  bool ok;
  uint value, length;
};

struct va_list_arg
{
  va_list args;
};

static struct console *print_console = null;
static spinlock        print_lock = { 0 };

static inline bool force_inline
isDigit (char ch)
{
  return ch >= '0' && ch <= '9';
}

static inline uint
parseFlags (struct format *format, const char *fmt, byte b)
{
  uint n = 0;

  for (;;)
    {
      switch (b)
        {
        case '-':
          format->minus = true;
          format->zero = false;
          break;
        case '+':
          format->plus = true;
          break;
        case ' ':
          format->space = true;
          break;
        case '#':
          format->hash = true;
          break;
        case '0':
          format->zero = !format->minus;
          break;
        case '^':
          format->caret = true;
          break;
        case '!':
          format->bang = true;
          break;
        default:
          return n;
        }

      b = fmt[++n];
    }
}

static inline struct arg
parseArg (const char *fmt, struct va_list_arg *va)
{
  struct arg arg = { 0 };
  char       ch = fmt[0];

  if (ch == '*')
    {
      arg.ok = true;
      arg.value = va_arg (va->args, uint);
      arg.length = 1;
      return arg;
    }

  if (!isDigit (ch))
    return arg;

  arg.ok = true;

  do
    {
      arg.value *= 10;
      arg.value += ch - '0';
      ch = fmt[++arg.length];
    }
  while (isDigit (ch));

  return arg;
}

static inline struct int_size force_inline
parseIntSize (const char *fmt)
{
  struct int_size size = { 0 };

  switch (fmt[0])
    {
    case '8':
      size.value = SIZE_8;
      size.length = 1;
      goto out;
    case '1':
      if (fmt[1] == '6')
        {
          size.value = SIZE_16;
          size.length = 2;
          goto out;
        }
      return size;
    case '3':
      if (fmt[1] == '2')
        {
          size.value = SIZE_32;
          size.length = 2;
          goto out;
        }
      return size;
    case '6':
      if (fmt[1] == '4')
        {
          size.value = SIZE_64;
          size.length = 2;
          goto out;
        }
      return size;
    default:
      return size;
    }

out:
  size.ok = true;
  return size;
}

static inline uint
fmtChar (struct writer writer, struct format *format, char ch)
{
  uint written = 0, pad = 0;

  if (format->width_set && format->width > 1)
    pad = format->width - 1;

  if (pad && !format->minus)
    written += writePad (writer, pad, ' ');

  written += writeChar (writer, ch);

  if (pad && format->minus)
    written += writePad (writer, pad, ' ');

  return written;
}

static inline uint
fmtString (struct writer writer, struct format *format, const char *s)
{
  uint written = 0, pad = 0, len = 0;

  if (format->prec_set)
    while (s[len] != '\0' && len < format->prec)
      ++len;
  else
    while (s[len] != '\0')
      ++len;

  if (format->width_set && format->width > len)
    pad = format->width - len;

  if (pad && !format->minus)
    written += writePad (writer, pad, ' ');

  written += writeStr (writer, len, s);

  if (pad && format->minus)
    written += writePad (writer, pad, ' ');

  return written;
}

static inline uint
fmtInt (struct writer writer, struct format *format, uint base, bool _signed,
        bool upper, u64 u)
{
#define CAP 64
  char buf[CAP], pbuf[8];
  uint digits = 0, plen = 0, written = 0;
  bool neg = false;

  if (base < 2 || base > 16)
    return 0;

  if (_signed && (i64) u < 0)
    {
      i64 i = (i64) u;

      if (i == INT64_MIN)
        u = (u64) INT64_MAX + 1;
      else
        u = (u64) -i;

      neg = true;
    }

  if (neg)
    pbuf[plen++] = '-';
  else if (format->plus)
    pbuf[plen++] = '+';
  else if (format->space)
    pbuf[plen++] = ' ';

  if (format->hash)
    {
      switch (base)
        {
        case 2:
          pbuf[plen++] = '0';
          pbuf[plen++] = format->caret ? 'B' : 'b';
          break;
        case 8:
          pbuf[plen++] = '0';
          pbuf[plen++] = format->caret ? 'O' : 'o';
          break;
        case 16:
          pbuf[plen++] = '0';
          pbuf[plen++] = format->caret ? 'X' : 'x';
          break;
        }
    }

  if (!u)
    {
      buf[CAP - 1] = '0';
      digits = 1;
    }

  while (u)
    {
      auto r = u % base;
      u /= base;
      buf[CAP - 1 - digits]
          = (upper ? "0123456789ABCDEF" : "0123456789abcdef")[r];
      digits += 1;
    }

  uint len = plen, zero_pad = 0, pad = 0;

  if (format->prec_set && format->prec > digits)
    {
      zero_pad = format->prec - digits;
      len += format->prec;
    }
  else
    len += digits;

  if (format->width_set && format->width > len)
    pad = format->width - len;

  if (pad && !format->minus)
    written += writePad (writer, pad, ' ');

  if (plen)
    written += writeStr (writer, plen, pbuf);

  if (zero_pad)
    written += writePad (writer, zero_pad, '0');

  written += writeStr (writer, digits, buf + (CAP - digits));

  if (pad && format->minus)
    written += writePad (writer, pad, ' ');

#undef CAP
  return written;
}

static inline u64 force_inline
getArg (struct va_list_arg *va, uint size, bool _signed)
{
#define SELECT(type1, type2)                                                  \
  do                                                                          \
    {                                                                         \
      if (_signed)                                                            \
        return va_arg (va->args, type1);                                      \
      else                                                                    \
        return va_arg (va->args, type2);                                      \
    }                                                                         \
  while (0)

  switch (size)
    {
    case SIZE_NONE:
    case SIZE_8:
    case SIZE_16:
      // C standard guarantees sizeof(int) at least 2.
      SELECT (int, uint);
    case SIZE_32:
    case SIZE_64:
      {
        uint s = (size == SIZE_32 ? 4 : 8);

        if (sizeof (int) >= s)
          SELECT (int, uint);
        else if (sizeof (long) >= s)
          SELECT (long, ulong);
        else if (sizeof (long long) >= s)
          SELECT (long long, ullong);
      }
    }

#undef SELECT
  return 0;
}

struct console *
getPrintConsole (void)
{
  return print_console;
}

void
setPrintConsole (struct console *console)
{
  print_console = console;
}

spinlock *
getPrintLock (void)
{
  return &print_lock;
}

uint
vwLog (struct writer writer, const char *fmt, va_list args)
{
  struct va_list_arg va;
  va_copy (va.args, args);

  uint n = 0, i = 0;

  struct format format;

  if (writer.write == null)
    writer.write = writeNil;

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
          n += writeChar (writer, '%');
          continue;
        }

      format = (struct format) { 0 };

      i += parseFlags (&format, fmt + i, b);
      b = fmt[i];

      struct arg width_arg = parseArg (fmt + i, &va);

      if (width_arg.ok)
        {
          format.width_set = true;
          format.width = width_arg.value;
          i += width_arg.length;
          b = fmt[i];
        }

      if (b == '.')
        {
          ++i;
          struct arg prec_arg = parseArg (fmt + i, &va);

          format.prec_set = true;

          if (prec_arg.ok)
            {
              format.prec = prec_arg.value;
              i += prec_arg.length;
            }

          b = fmt[i];
        }

      uint base = 10, size = SIZE_NONE;
      bool _signed = false, upper = false;

      switch (b)
        {
        case 'c':
          n += fmtChar (writer, &format, va_arg (va.args, int));
          continue;
        case 's':
          n += fmtString (writer, &format, va_arg (va.args, const char *));
          continue;
        case 't':
          n += fmtString (writer, &format,
                          va_arg (va.args, int) ? "true" : "false");
          continue;
        case 'i':
          _signed = true;
          break;
        case 'd':
          _signed = true;
          break;
        case 'b':
          base = 2;
          break;
        case 'o':
          base = 8;
          break;
        case 'X':
          upper = true;
        case 'x':
          base = 16;
          break;
        case 'u':
          break;
        case 'P':
          upper = true;
        case 'p':
          format.hash = !format.hash;
          n += fmtInt (writer, &format, 16, false, upper,
                       (uintptr) va_arg (va.args, void *));
          continue;
        default:
          continue;
        }

      if (size == SIZE_NONE && !format.bang)
        {
          struct int_size isize = parseIntSize (fmt + i + 1);

          if (isize.ok)
            {
              size = isize.value;
              i += isize.length;
            }
        }

      n += fmtInt (writer, &format, base, _signed, upper,
                   getArg (&va, size, _signed));
    }

  va_end (va.args);
  return n;
}