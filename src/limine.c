#include "loom/limine.h"
#include "loom/mmap.h"
#include "loom/time.h"

static used u64 start_marker[4] = LIMINE_REQUESTS_START_MARKER;

static used struct limine_framebuffer_request framebuffer_request
    = { .id = LIMINE_FRAMEBUFFER_REQUEST_ID };

static used struct limine_memmap_request memmap_request
    = { .id = LIMINE_MEMMAP_REQUEST_ID };

static used struct limine_date_at_boot_request date_at_boot_request
    = { .id = LIMINE_DATE_AT_BOOT_REQUEST_ID };

static used u64 end_marker[4] = LIMINE_REQUESTS_END_MARKER;

bool
limine_get_framebuffers (u64                         *framebuffer_count,
                         struct limine_framebuffer ***framebuffers)
{
  if (framebuffer_request.response == null)
    return false;
  auto response = framebuffer_request.response;
  *framebuffer_count = response->framebuffer_count;
  *framebuffers = response->framebuffers;
  return true;
}

bool
limine_get_boot_time (timestamp *ts)
{
  if (date_at_boot_request.response == null)
    return false;
  *ts = date_at_boot_request.response->timestamp;
  return true;
}

static void
kprint_init (void)
{
  u64                         framebuffer_count;
  struct limine_framebuffer **framebuffers;
  struct limine_framebuffer  *framebuffer;

  if (!limine_get_framebuffers (&framebuffer_count, &framebuffers)
      || !framebuffer_count)
    return;

  framebuffer = framebuffers[0];
  if (framebuffer == null)
    return;

  auto early_console = early_limine_gfx_console_create (framebuffer);
  set_print_console (early_console);
}

static int
limine_memmap_iterate (mmap_iterator_hook hook, void *ctx)
{
  int retval;

  if (memmap_request.response == null)
    return 0;

  auto response = memmap_request.response;

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
limine_early_boot (void)
{
  struct boot_info bi;

  kprint_init ();

  bi.boot_time_set = limine_get_boot_time (&bi.boot_time);
  bi.mmap_iterator = limine_memmap_iterate;

  return bi;
}