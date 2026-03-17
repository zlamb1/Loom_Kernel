#ifndef LOOM_PRINT_H
#define LOOM_PRINT_H 1

#include <stdarg.h>

#include "loom/compiler.h"
#include "loom/console.h"
#include "loom/writer.h"

struct console *getPrintConsole (void);
void            setPrintConsole (struct console *);

uint vwLog (struct writer writer, const char *fmt, va_list args);

static inline struct writer
getPrintWriter (void)
{
  struct console *print_console = getPrintConsole ();
  struct writer   writer = { 0 };

  if (print_console != null)
    {
      writer.write = print_console->write;
      writer.data = print_console->data;
    }

  return writer;
}

static inline uint
wLog (struct writer writer, const char *fmt, ...)
{
  uint    n;
  va_list args;
  va_start (args, fmt);
  n = vwLog (writer, fmt, args);
  va_end (args);
  return n;
}

static inline uint force_inline
vwLogLn (struct writer writer, const char *fmt, va_list args)
{
  return vwLog (writer, fmt, args) + wLog (writer, "\n");
}

static inline uint
wLogLn (struct writer writer, const char *fmt, ...)
{
  uint    n;
  va_list args;
  va_start (args, fmt);
  n = vwLogLn (writer, fmt, args);
  va_end (args);
  return n;
}

static inline uint
vkLog (const char *fmt, va_list args)
{
  return vwLog (getPrintWriter (), fmt, args);
}

static inline uint
vkLogLn (const char *fmt, va_list args)
{
  return vwLogLn (getPrintWriter (), fmt, args);
}

static inline uint
kLog (const char *fmt, ...)
{
  uint    n;
  va_list args;
  va_start (args, fmt);
  n = vkLog (fmt, args);
  va_end (args);
  return n;
}

static inline uint
kLogLn (const char *fmt, ...)
{
  uint    n;
  va_list args;
  va_start (args, fmt);
  n = vkLogLn (fmt, args);
  va_end (args);
  return n;
}

#endif