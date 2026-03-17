#ifndef LOOM_BOOT_INFO_H
#define LOOM_BOOT_INFO_H 1

#include "loom/arch.h"
#include "loom/mmap.h"
#include "loom/time.h"

typedef void (*mp_bootstrap_func) (cpu_handle);

struct boot_request
{
  mp_bootstrap_func bs_func;
};

struct boot_info
{
  bool      boot_time_set : 1;
  timestamp boot_time;
  int (*mmap_iterator) (mmap_iterator_hook hook, void *ctx);
};

#endif