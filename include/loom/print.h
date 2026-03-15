#ifndef LOOM_PRINT_H
#define LOOM_PRINT_H 1

#include <stdarg.h>

#include "loom/compiler.h"
#include "loom/console.h"
#include "loom/writer.h"

struct console *get_print_console (void);
void            set_print_console (struct console *);

uint vwprintf (struct writer writer, const char *fmt, va_list args);

static inline uint
wprintf (struct writer writer, const char *fmt, ...)
{
  uint    n;
  va_list args;
  va_start (args, fmt);
  n = vwprintf (writer, fmt, args);
  va_end (args);
  return n;
}

static inline uint force_inline
vwprintfln (struct writer writer, const char *fmt, va_list args)
{
  return vwprintf (writer, fmt, args) + wprintf (writer, "\n");
}

static inline uint
wprintfln (struct writer writer, const char *fmt, ...)
{
  uint    n;
  va_list args;
  va_start (args, fmt);
  n = vwprintfln (writer, fmt, args);
  va_end (args);
  return n;
}

static inline uint
vkprintf (const char *fmt, va_list args)
{
  struct console *print_console = get_print_console ();
  struct writer   writer = { 0 };

  if (print_console != null)
    {
      writer.write = print_console->write;
      writer.data = print_console->data;
    }

  return vwprintf (writer, fmt, args);
}

static inline uint
vkprintfln (const char *fmt, va_list args)
{
  struct console *print_console = get_print_console ();
  struct writer   writer = { 0 };

  if (print_console != null)
    {
      writer.write = print_console->write;
      writer.data = print_console->data;
    }

  return vwprintfln (writer, fmt, args);
}

static inline uint
kprintf (const char *fmt, ...)
{
  uint    n;
  va_list args;
  va_start (args, fmt);
  n = vkprintf (fmt, args);
  va_end (args);
  return n;
}

static inline uint
kprintfln (const char *fmt, ...)
{
  uint    n;
  va_list args;
  va_start (args, fmt);
  n = vkprintfln (fmt, args);
  va_end (args);
  return n;
}

#endif