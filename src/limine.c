#include "loom/limine.h"
#include "loom/compiler.h"
#include "loom/time.h"

static used u64 start_marker[4] = LIMINE_REQUESTS_START_MARKER;

static used struct limine_framebuffer_request framebuffer_request
    = { .id = LIMINE_FRAMEBUFFER_REQUEST_ID };

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

struct boot_info
limine_early_boot (void)
{
  struct boot_info bi;

  bi.boot_time_set = limine_get_boot_time (&bi.boot_time);
  kprint_init ();

  return bi;
}