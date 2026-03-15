#ifndef LOOM_BOOT_INFO_H
#define LOOM_BOOT_INFO_H 1

#include "loom/time.h"

struct boot_info
{
  bool      boot_time_set : 1;
  timestamp boot_time;
};

#endif