#include "loom/limine.h"
#include "loom/mmap.h"
#include "loom/print.h"
#include "loom/time.h"

// Prevent the compiler from reordering markers/requests by wrapping in a
// struct.

struct
{
  u64                                start_marker[4];
  struct limine_framebuffer_request  fb;
  struct limine_memmap_request       memmap;
  struct limine_date_at_boot_request date_at_boot;
  u64                                end_marker[4];
} static volatile used section (".limine") requests = {
  .start_marker = LIMINE_REQUESTS_START_MARKER,
  .fb = { .id = LIMINE_FRAMEBUFFER_REQUEST_ID },
  .memmap = { .id = LIMINE_MEMMAP_REQUEST_ID },
  .date_at_boot = { .id = LIMINE_DATE_AT_BOOT_REQUEST_ID },
  .end_marker = LIMINE_REQUESTS_END_MARKER,
};

bool
limineGetFramebuffers (u64                         *framebuffer_count,
                       struct limine_framebuffer ***framebuffers)
{
  if (requests.fb.response == null)
    return false;
  auto response = requests.fb.response;
  *framebuffer_count = response->framebuffer_count;
  *framebuffers = response->framebuffers;
  return true;
}

bool
limineGetBootTime (timestamp *ts)
{
  if (requests.date_at_boot.response == null)
    return false;
  *ts = requests.date_at_boot.response->timestamp;
  return true;
}

static void
printInit (void)
{
  u64                         framebuffer_count;
  struct limine_framebuffer **framebuffers;
  struct limine_framebuffer  *framebuffer;

  if (!limineGetFramebuffers (&framebuffer_count, &framebuffers)
      || !framebuffer_count)
    return;

  framebuffer = framebuffers[0];
  if (framebuffer == null)
    return;

  auto early_console = earlyLimineGfxConsoleCreate (framebuffer);
  setPrintConsole (early_console);
}

static int
limineMmapIterate (mmap_iterator_hook hook, void *ctx)
{
  int retval;

  if (requests.memmap.response == null)
    return 0;

  auto response = requests.memmap.response;

  for (uint i = 0; i < response->entry_count; ++i)
    {
      auto entry = response->entries[i];
      auto type = entry->type;

      switch (type)
        {
        case LIMINE_MEMMAP_USABLE:
          type = MEMORY_TYPE_FREE;
          break;
        case LIMINE_MEMMAP_BAD_MEMORY:
          type = MEMORY_TYPE_BAD_RAM;
          break;
        default:
          type = MEMORY_TYPE_RESERVED;
          break;
        }

      if ((retval = hook (entry->base, entry->length, type, ctx)))
        return retval;
    }

  return 0;
}

struct boot_info
limineEarlyBoot (void)
{
  struct boot_info bi;

  printInit ();

  bi.boot_time_set = limineGetBootTime (&bi.boot_time);
  bi.mmap_iterator = limineMmapIterate;

  return bi;
}