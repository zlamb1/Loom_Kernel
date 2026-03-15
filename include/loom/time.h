#ifndef LOOM_TIME_H
#define LOOM_TIME_H 1

#include <stdarg.h>

#include "loom/print.h"

// Unix timestamp.
typedef i64 timestamp;

struct datetime
{
  uint sec;
  uint min;
  uint hour;
  uint week_day;
  uint month_day;
  uint year_day;
  uint month;
  int  year;
};

struct datetime mk_datetime (timestamp ts);

uint ts_wprintf (struct writer writer, timestamp ts, const char *fmt);

static inline uint
ts_kprintf (timestamp ts, const char *fmt)
{
  return ts_wprintf (get_print_writer (), ts, fmt);
}

static inline uint
ts_kprintfln (timestamp ts, const char *fmt)
{
  auto w = get_print_writer ();
  return ts_wprintf (w, ts, fmt) + wprintf (w, "\n");
}

#endif