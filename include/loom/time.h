#include "loom/sync.h"
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

struct datetime mkDatetime (timestamp ts);

uint tsFormat (struct writer writer, timestamp ts, const char *fmt);

static inline uint
tsLog (timestamp ts, const char *fmt)
{
  auto lock = getPrintLock ();
  auto flags = lockGetIrq (lock);
  auto n = tsFormat (getPrintWriter (), ts, fmt);
  lockPutIrq (lock, flags);
  return n;
}

static inline uint
tsLogLn (timestamp ts, const char *fmt)
{
  auto w = getPrintWriter ();
  auto lock = getPrintLock ();
  auto flags = lockGetIrq (lock);
  auto n = tsFormat (w, ts, fmt) + wLog (w, "\n");
  lockPutIrq (lock, flags);
  return n;
}

#endif