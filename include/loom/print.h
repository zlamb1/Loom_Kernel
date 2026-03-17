#ifndef LOOM_PRINT_H
#define LOOM_PRINT_H 1

#include <stdarg.h>

#include "loom/console.h"
#include "loom/sync.h"
#include "loom/writer.h"

struct console *getPrintConsole (void);
void            setPrintConsole (struct console *);

spinlock *getPrintLock (void);

uint vwLog (struct writer writer, const char *fmt, va_list args);

static inline struct writer
getPrintWriter (void)
{
  auto          print_console = getPrintConsole ();
  struct writer writer = { 0 };

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
  va_list args;
  va_start (args, fmt);
  auto n = vwLog (writer, fmt, args);
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
  va_list args;
  va_start (args, fmt);
  auto n = vwLogLn (writer, fmt, args);
  va_end (args);
  return n;
}

static inline uint
vkLog (const char *fmt, va_list args)
{
  auto lock = getPrintLock ();
  auto flags = lockGetIrq (lock);
  auto n = vwLog (getPrintWriter (), fmt, args);
  lockPutIrq (lock, flags);
  return n;
}

static inline uint
vkLogLn (const char *fmt, va_list args)
{
  auto lock = getPrintLock ();
  auto flags = lockGetIrq (lock);
  auto n = vwLogLn (getPrintWriter (), fmt, args);
  lockPutIrq (lock, flags);
  return n;
}

static inline uint
kLog (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  auto n = vkLog (fmt, args);
  va_end (args);
  return n;
}

static inline uint
kLogLn (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  auto n = vkLogLn (fmt, args);
  va_end (args);
  return n;
}

#endif