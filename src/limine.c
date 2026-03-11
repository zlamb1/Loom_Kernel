#include "loom/limine.h"
#include "loom/compiler.h"

static used u64 start_marker[4] = LIMINE_REQUESTS_START_MARKER;

static used struct limine_framebuffer_request framebuffer_request
    = { .id = LIMINE_FRAMEBUFFER_REQUEST_ID };

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
