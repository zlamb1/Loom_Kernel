#include "loom/time.h"

#define SECONDS_PER_HOUR      (60 * 60)
#define SECONDS_PER_DAY       (60 * 60 * 24)
#define SECONDS_PER_YEAR      (SECONDS_PER_DAY * 365)
#define SECONDS_PER_LEAP_YEAR (SECONDS_PER_DAY * 366)

const int days_per_month[] = {
  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
};

const int days_per_month_leap[] = {
  31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
};

const char *week_day_names[7] = {
  "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday",
};

const char *month_names[12] = {
  "January", "February",  "March",   "April",    "June",     "July",
  "August",  "September", "October", "November", "December",
};

const char *abbrev_month_names[12] = {
  "Jan", "Feb", "Mar", "Apr", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

static inline bool force_inline
isLeapYear (int year)
{
  if (year & 3)
    return false;

  if (!(year % 100) && (year % 400))
    return false;

  return true;
}

struct datetime
mkDatetime (timestamp ts)
{
  struct datetime datetime;
  i64             days = ts / SECONDS_PER_DAY;
  int             year = 1970;
  bool            leap;

  if (ts < 0)
    {
      int next, seconds;

      if (ts % SECONDS_PER_DAY)
        // If we are 1 second after Thursday, its still Thursday.
        // But if we are 1 second before Thursday, it should be Wednesday.
        days -= 1;

      for (;;)
        {
          next = year - 1;
          seconds
              = isLeapYear (next) ? SECONDS_PER_LEAP_YEAR : SECONDS_PER_YEAR;

          if (-ts < seconds)
            break;

          ts += seconds;
          year = next;
        }

      if (ts)
        {
          // Normalize the timestamp.
          ts = seconds + ts;
          year = next;
        }
    }
  else
    for (;;)
      {
        auto seconds
            = isLeapYear (year) ? SECONDS_PER_LEAP_YEAR : SECONDS_PER_YEAR;

        if (ts < seconds)
          break;

        ts -= seconds;
        year += 1;
      }

  leap = isLeapYear (year);

  // Jan. 1 1970 = Thursday
  auto week_day = (4 + days) % 7;
  if (week_day < 0)
    week_day += 7;

  datetime.week_day = week_day;
  datetime.year_day = ts / SECONDS_PER_DAY;
  datetime.year = year;

  auto dpm = leap ? days_per_month_leap : days_per_month;

  datetime.month = 0;

  for (int i = 0; i < 12; ++i)
    {
      auto seconds = SECONDS_PER_DAY * dpm[i];
      if (ts < seconds)
        break;
      ts -= seconds;
      datetime.month += 1;
    }

  datetime.month_day = ts / SECONDS_PER_DAY;
  ts %= SECONDS_PER_DAY;

  datetime.hour = ts / SECONDS_PER_HOUR;
  ts %= SECONDS_PER_HOUR;

  datetime.min = ts / 60;
  ts %= 60;

  datetime.sec = ts;

  return datetime;
}

uint
tsFormat (struct writer writer, timestamp ts, const char *fmt)
{
  uint n = 0, i = 0;

  auto dt = mkDatetime (ts);
  auto hour_12 = ((dt.hour + 11) % 12) + 1;

  if (writer.write == null)
    writer.write = writeNil;

  for (;; ++i)
    {
      byte b = fmt[i];
      uint start = i;

      while (b != '\0' && b != '%')
        b = fmt[++i];

      if (start < i)
        n += writeStr (writer, i - start, fmt + start);

      if (fmt[i] == '\0')
        break;

      b = fmt[++i];

      if (b == '%')
        {
          n += writeChar (writer, '%');
          continue;
        }

      switch (b)
        {
        case 'n':
          n += writeChar (writer, '\n');
          break;
        case 'Y':
          n += wLog (writer, "%.2i", dt.year);
          break;
        case 'y':
          n += wLog (writer, "%.2i", dt.year % 100);
          break;
        case 'C':
          n += wLog (writer, "%.2i", dt.year / 100);
          break;
        case 'b':
        case 'h':
          n += wLog (writer, "%s", abbrev_month_names[dt.month]);
          break;
        case 'B':
          n += wLog (writer, "%s", month_names[dt.month]);
          break;
        case 'm':
          n += wLog (writer, "%u", dt.month + 1);
          break;
        case 'j':
          n += wLog (writer, "%u", dt.year_day + 1);
          break;
        case 'd':
          n += wLog (writer, "%u", dt.month_day + 1);
          break;
        case 'e':
          n += wLog (writer, "%2u", dt.month_day + 1);
          break;
        case 'A':
          n += wLog (writer, "%s", week_day_names[dt.week_day]);
          break;
        case 'w':
          n += wLog (writer, "%u", dt.week_day);
          break;
        case 'H':
          n += wLog (writer, "%.2u", dt.hour);
          break;
        case 'I':
          n += wLog (writer, "%.2u", hour_12);
          break;
        case 'M':
          n += wLog (writer, "%.2u", dt.min);
          break;
        case 'S':
          n += wLog (writer, "%.2u", dt.sec);
          break;
        case 'p':
          n += wLog (writer, "%s", dt.hour >= 12 ? "p.m." : "a.m.");
          break;
        case 'P':
          n += wLog (writer, "%s", dt.hour >= 12 ? "PM" : "AM");
          break;
        }
    }

  return n;
}